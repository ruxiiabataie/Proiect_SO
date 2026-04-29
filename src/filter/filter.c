#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "filter.h"

// ----------------------
// PARSE
// ----------------------

int parse_condition(const char *input, char *field, char *op, char *value) {
    return sscanf(input, "%[^:]:%[^:]:%s", field, op, value);
}

// ----------------------
// MATCH
// ----------------------

int match_int(int a, const char *op, int b) {
    if (strcmp(op, "==") == 0) return a == b;
    if (strcmp(op, "!=") == 0) return a != b;
    if (strcmp(op, ">") == 0) return a > b;
    if (strcmp(op, "<") == 0) return a < b;
    if (strcmp(op, ">=") == 0) return a >= b;
    if (strcmp(op, "<=") == 0) return a <= b;
    return 0;
}

int match_condition(Report *r, const char *field, const char *op, const char *value) {

    if (strcmp(field, "severity") == 0) {
        int val = atoi(value);
        return match_int(r->severity, op, val);
    }

    if (strcmp(field, "category") == 0) {
        if (strcmp(op, "==") == 0) return strcmp(r->category, value) == 0;
        if (strcmp(op, "!=") == 0) return strcmp(r->category, value) != 0;
    }

    if (strcmp(field, "inspector") == 0) {
        if (strcmp(op, "==") == 0) return strcmp(r->inspector, value) == 0;
        if (strcmp(op, "!=") == 0) return strcmp(r->inspector, value) != 0;
    }

    if (strcmp(field, "timestamp") == 0) {
        int val = atoi(value);
        return match_int((int)r->timestamp, op, val);
    }

    return 0;
}