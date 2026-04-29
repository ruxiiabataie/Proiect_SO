#ifndef PERMISSIONS_H
#define PERMISSIONS_H
#include <sys/stat.h>

int check_read_permission(const char *path, const char *role);
int check_write_permission(const char *path, const char *role);
void set_permissions(const char *path, mode_t mode);

#endif