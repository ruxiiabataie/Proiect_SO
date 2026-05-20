#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "models/report.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("No district provided\n");

        return 1;
    }

    char path[256];

    snprintf(path,
             sizeof(path),
             "%s/reports.dat",
             argv[1]);

    FILE *fp = fopen(path, "rb");

    if (fp == NULL)
    {
        perror("fopen");

        return 1;
    }

    Report r;

    char inspectors[100][50];

    int scores[100];

    int count = 0;

    while (fread(&r,
                 sizeof(Report),
                 1,
                 fp) == 1)
    {
        int found = 0;

        for (int i = 0; i < count; i++)
        {
            if (strcmp(inspectors[i],
                       r.inspector) == 0)
            {
                scores[i] += r.severity;

                found = 1;

                break;
            }
        }

        if (!found)
        {
            strcpy(inspectors[count],
                   r.inspector);

            scores[count] = r.severity;

            count++;
        }
    }

    fclose(fp);

    printf("District: %s\n\n",
           argv[1]);

    for (int i = 0; i < count; i++)
    {
        printf("%s -> %d\n",
               inspectors[i],
               scores[i]);
    }

    return 0;
}