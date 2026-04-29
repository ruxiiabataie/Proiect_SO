#ifndef FILE_OPS_H
#define FILE_OPS_H

#include "../models/report.h"

// path helpers
void build_reports_path(char *dest, const char *district);

// file operations
int open_reports_file(const char *district, int flags);

// report operations
int append_report(const char *district, Report *r);
int read_all_reports(const char *district, void (*callback)(Report*));
int find_report(const char *district, int id, Report *out);
int remove_report_file(const char *district, int id);

#endif