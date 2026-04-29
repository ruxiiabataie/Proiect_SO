#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>

#include "commands.h"
#include "../models/report.h"
#include "../file/file_ops.h"
#include "../filter/filter.h"
#include "../permissions/permissions.h"


#define REPORTS_FILE "reports.dat"
#define CFG_FILE "district.cfg"

// ----------------------
// HELPERS
// ----------------------

void build_path(char *dest, const char *district, const char *file) {
    snprintf(dest, 256, "%s/%s", district, file);
}

void ensure_directory(const char *district) {
    mkdir(district, 0750);
}

// ----------------------
// ADD
// ----------------------
void log_action(const char *district, const char *role, const char *user, const char *action) {
    char path[256];
    snprintf(path, sizeof(path), "%s/logged_district", district);

    int fd = open(path, O_WRONLY | O_APPEND);
    if (fd >= 0) {
        time_t now = time(NULL);
        dprintf(fd, "%ld | %s | %s | %s\n", now, role, user, action);
        close(fd);
    }
}

void check_symlink(const char *link_name) {
    struct stat st;

    // verifică link-ul însuși
    if (lstat(link_name, &st) < 0) {
        perror("lstat");
        return;
    }

    if (S_ISLNK(st.st_mode)) {
        printf("[SYMLINK] %s exists and is a symbolic link\n", link_name);

        // verifică dacă target-ul există
        if (stat(link_name, &st) < 0) {
            printf("[WARNING] Dangling symlink detected: %s\n", link_name);
        }
    }
}

void handle_add(const char *role, const char *user, const char *district) {
    ensure_directory(district);

    char cfg_path[256];
    char log_path[256];
    char reports_path[256];
    char link_name[256];

    snprintf(cfg_path, sizeof(cfg_path), "%s/district.cfg", district);
    snprintf(log_path, sizeof(log_path), "%s/logged_district", district);
    build_path(reports_path, district, REPORTS_FILE);

    // ----------------------
    // CREATE FILES IF NEEDED
    // ----------------------

    if (access(cfg_path, F_OK) != 0) {
        int fd_cfg = open(cfg_path, O_CREAT | O_RDWR, 0640);
        if (fd_cfg >= 0) {
            dprintf(fd_cfg, "threshold=2\n");
            close(fd_cfg);
            chmod(cfg_path, 0640);
        }
    }

    if (access(log_path, F_OK) != 0) {
        int fd_log = open(log_path, O_CREAT | O_RDWR, 0644);
        if (fd_log >= 0) {
            close(fd_log);
            chmod(log_path, 0644);
        }
    }

    int fd_tmp = open(reports_path, O_CREAT, 0664);
    if (fd_tmp >= 0) {
        close(fd_tmp);
        chmod(reports_path, 0664);
    }

    // ----------------------
    // PERMISSION CHECK
    // ----------------------

    if (!check_write_permission(reports_path, role)) {
        printf("Permission denied (write)\n");
        return;
    }

    // ----------------------
    // BUILD REPORT (INPUT)
    // ----------------------

    Report r;

    r.id = rand() % 100000;

    strncpy(r.inspector, user, sizeof(r.inspector) - 1);
    r.inspector[sizeof(r.inspector) - 1] = '\0';

    printf("Latitude: ");
    scanf("%f", &r.latitude);

    printf("Longitude: ");
    scanf("%f", &r.longitude);

    printf("Category: ");
    scanf("%s", r.category);

    printf("Severity (1-3): ");
    scanf("%d", &r.severity);

    printf("Description: ");
    scanf(" %[^\n]", r.description);

    r.timestamp = time(NULL);

    // ----------------------
    // WRITE REPORT
    // ----------------------

    if (append_report(district, &r) < 0) {
        perror("append_report");
        return;
    }

    printf("[ADD] Report created with ID %d\n", r.id);

    // ----------------------
    // SYMLINK
    // ----------------------

    snprintf(link_name, sizeof(link_name), "active_reports-%s", district);
    unlink(link_name); // remove old if exists
    symlink(reports_path, link_name);

    check_symlink(link_name);

    // ----------------------
    // LOGGING
    // ----------------------

    if (strcmp(role, "manager") == 0) 
        log_action(district, role, user, "ADD");

}
// ----------------------
// LIST
// ----------------------

void print_report(Report *r) {
    printf("ID: %d | Inspector: %s | Category: %s | Severity: %d\n",
           r->id, r->inspector, r->category, r->severity);
}

void permissions_to_string(mode_t mode, char *perm) {
    perm[0] = (mode & S_IRUSR) ? 'r' : '-';
    perm[1] = (mode & S_IWUSR) ? 'w' : '-';
    perm[2] = (mode & S_IXUSR) ? 'x' : '-';

    perm[3] = (mode & S_IRGRP) ? 'r' : '-';
    perm[4] = (mode & S_IWGRP) ? 'w' : '-';
    perm[5] = (mode & S_IXGRP) ? 'x' : '-';

    perm[6] = (mode & S_IROTH) ? 'r' : '-';
    perm[7] = (mode & S_IWOTH) ? 'w' : '-';
    perm[8] = (mode & S_IXOTH) ? 'x' : '-';

    perm[9] = '\0';
}

void handle_list(const char *role, const char *district) {
    char path[256];
    build_path(path, district, REPORTS_FILE);

    if (!check_read_permission(path, role)) {
        printf("Permission denied (read)\n");
        return;
    }

    struct stat st;

    if (stat(path, &st) < 0) {
        perror("stat");
        return;
    }

    char perms[10];
    permissions_to_string(st.st_mode, perms);

    char *time_str = ctime(&st.st_mtime);
    if (time_str[strlen(time_str) - 1] == '\n') {
        time_str[strlen(time_str) - 1] = '\0';
    }

    printf("\n=== REPORTS in %s ===\n", district);
    printf("File: %s\n", path);
    printf("Permissions: %s\n", perms);
    printf("Size: %lld bytes\n", st.st_size);
    printf("Last modified: %s\n\n", time_str);

    if (read_all_reports(district, print_report) < 0) {
        perror("read_all_reports");
    }
}

// ----------------------
// VIEW
// ----------------------

void handle_view(const char *role, const char *district, int report_id) {
    char path[256];
    build_path(path, district, REPORTS_FILE);

    if (!check_read_permission(path, role)) {
        printf("Permission denied (read)\n");
        return;
    }

    Report r;

    if (find_report(district, report_id, &r)) {
        printf("\n=== REPORT %d ===\n", report_id);
        printf("Inspector: %s\n", r.inspector);
        printf("Location: %.2f %.2f\n", r.latitude, r.longitude);
        printf("Category: %s\n", r.category);
        printf("Severity: %d\n", r.severity);
        printf("Description: %s\n", r.description);
    } else {
        printf("Report not found\n");
    }
}

// ----------------------
// REMOVE
// ----------------------

void handle_remove(const char *role, const char *district, int report_id) {
    if (strcmp(role, "manager") != 0) {
        printf("Permission denied: only manager can remove reports\n");
        return;
    }

    char path[256];
    build_path(path, district, REPORTS_FILE);

    if (!check_write_permission(path, role)) {
        printf("Permission denied (write)\n");
        return;
    }

    if (remove_report_file(district, report_id)) {
        printf("[REMOVE] Report %d deleted\n", report_id);
    } else {
        printf("Report not found\n");
    }
}

// ----------------------
// UPDATE THRESHOLD
// ----------------------

void handle_update_threshold(const char *role, const char *district, int value) {
    if (strcmp(role, "manager") != 0) {
        printf("Permission denied\n");
        return;
    }

    char path[256];
    build_path(path, district, CFG_FILE);

    struct stat st;

    if (stat(path, &st) == 0) {
        if ((st.st_mode & 0777) != 0640) {
            printf("Invalid permissions on cfg file\n");
            return;
        }
    }

    int fd = open(path, O_CREAT | O_WRONLY | O_TRUNC, 0640);
    if (fd < 0) {
        perror("open");
        return;
    }

    dprintf(fd, "threshold=%d\n", value);
    close(fd);

    chmod(path, 0640);

    printf("[CFG] Threshold updated to %d\n", value);
}

// ----------------------
// FILTER
// ----------------------

void handle_filter(const char *role, const char *district, int argc, char *argv[], int start_index) {
    char path[256];
    build_path(path, district, REPORTS_FILE);

    if (!check_read_permission(path, role)) {
        printf("Permission denied (read)\n");
        return;
    }

    int fd = open_reports_file(district, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return;
    }

    Report r;
    char field[50], op[10], value[50];

    while (read(fd, &r, sizeof(Report)) > 0) {

        int ok = 1;

        for (int i = start_index; i < argc; i++) {
            parse_condition(argv[i], field, op, value);

            if (!match_condition(&r, field, op, value)) {
                ok = 0;
                break;
            }
        }

        if (ok) {
            printf("ID: %d | Inspector: %s | Category: %s | Severity: %d\n",
                   r.id, r.inspector, r.category, r.severity);
        }
    }

    close(fd);
}