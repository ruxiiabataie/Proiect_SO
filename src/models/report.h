#ifndef REPORT_H
#define REPORT_H

#include <time.h>

typedef struct {
    int id;
    char inspector[50];
    float latitude;
    float longitude;
    char category[30];
    int severity;
    time_t timestamp;
    char description[256];
} Report;

#endif