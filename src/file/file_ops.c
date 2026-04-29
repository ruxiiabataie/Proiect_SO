
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#include "file_ops.h"

#define REPORTS_FILE "reports.dat"

// ----------------------
// PATH
// ----------------------

void build_reports_path(char *dest, const char *district) {
    snprintf(dest, 256, "%s/%s", district, REPORTS_FILE);
}

// ----------------------
// OPEN
// ----------------------

int open_reports_file(const char *district, int flags) {
    char path[256];
    build_reports_path(path, district);

    return open(path, flags, 0664);
}

// ----------------------
// APPEND
// ----------------------

int append_report(const char *district, Report *r) {
    int fd = open_reports_file(district, O_CREAT | O_WRONLY | O_APPEND);
    if (fd < 0) return -1;

    int res = write(fd, r, sizeof(Report));

    close(fd);
    return res;
}

// ----------------------
// READ ALL
// ----------------------

int read_all_reports(const char *district, void (*callback)(Report*)) {
    int fd = open_reports_file(district, O_RDONLY);
    if (fd < 0) return -1;

    Report r;

    while (read(fd, &r, sizeof(Report)) > 0) {
        callback(&r);
    }

    close(fd);
    return 0;
}

// ----------------------
// FIND
// ----------------------

int find_report(const char *district, int id, Report *out) {
    int fd = open_reports_file(district, O_RDONLY);
    if (fd < 0) return -1;

    Report r;

    while (read(fd, &r, sizeof(Report)) > 0) {
        if (r.id == id) {
            *out = r;
            close(fd);
            return 1;
        }
    }

    close(fd);
    return 0;
}

// ----------------------
// REMOVE (important)
// ----------------------

int remove_report_file(const char *district, int id) {
    int fd = open_reports_file(district, O_RDWR);
    if (fd < 0) return -1;

    Report r;
    off_t pos = 0;

    while (read(fd, &r, sizeof(Report)) > 0) {

        if (r.id == id) {

            off_t next_pos = lseek(fd, 0, SEEK_CUR);
            Report temp;

            while (read(fd, &temp, sizeof(Report)) > 0) {
                lseek(fd, next_pos - sizeof(Report), SEEK_SET);
                write(fd, &temp, sizeof(Report));
                next_pos += sizeof(Report);
                lseek(fd, next_pos, SEEK_SET);
            }

            ftruncate(fd, next_pos - sizeof(Report));
            close(fd);
            return 1;
        }

        pos += sizeof(Report);
    }

    close(fd);
    return 0;
}