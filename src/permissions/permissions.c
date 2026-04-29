#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

#include "permissions.h"

// ----------------------
// SET PERMISSIONS
// ----------------------

void set_permissions(const char *path, mode_t mode) {
    if (chmod(path, mode) < 0) {
        perror("chmod");
    }
}

// ----------------------
// CHECK READ
// ----------------------

int check_read_permission(const char *path, const char *role) {
    struct stat st;

    if (stat(path, &st) < 0) {
        perror("stat");
        return 0;
    }

    if (strcmp(role, "manager") == 0) {
        return (st.st_mode & S_IRUSR);
    }

    if (strcmp(role, "inspector") == 0) {
        return (st.st_mode & S_IRGRP);
    }

    return 0;
}

// ----------------------
// CHECK WRITE
// ----------------------

int check_write_permission(const char *path, const char *role) {
    struct stat st;

    if (stat(path, &st) < 0) {
        perror("stat");
        return 0;
    }

    if (strcmp(role, "manager") == 0) {
        return (st.st_mode & S_IWUSR);
    }

    if (strcmp(role, "inspector") == 0) {
        return (st.st_mode & S_IWGRP);
    }

    return 0;
}