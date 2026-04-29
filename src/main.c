#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "commands/commands.h"

int main(int argc, char *argv[]) {

    srand(time(NULL));
    if (argc < 7) {
        printf("Usage:\n");
        printf("./city_manager --role <role> --user <user> <command> <district> [args]\n");
        return 1;
    }

    char *role = argv[2];
    char *user = argv[4];
    char *command = argv[5];
    char *district = argv[6];

    // ----------------------
    // ADD
    // ----------------------
    if (strcmp(command, "--add") == 0) {
        handle_add(role, user, district);
    }

    // ----------------------
    // LIST
    // ----------------------
    else if (strcmp(command, "--list") == 0) {
        handle_list(role, district);
    }

    // ----------------------
    // VIEW
    // ----------------------
    else if (strcmp(command, "--view") == 0) {
        if (argc < 8) {
            printf("Usage: --view <district> <report_id>\n");
            return 1;
        }

        int id = atoi(argv[7]);
        handle_view(role, district, id);
    }

    // ----------------------
    // REMOVE
    // ----------------------
    else if (strcmp(command, "--remove_report") == 0) {
        if (argc < 8) {
            printf("Usage: --remove_report <district> <report_id>\n");
            return 1;
        }

        int id = atoi(argv[7]);
        handle_remove(role, district, id);
    }

    // ----------------------
    // UPDATE THRESHOLD
    // ----------------------
    else if (strcmp(command, "--update_threshold") == 0) {
        if (argc < 8) {
            printf("Usage: --update_threshold <district> <value>\n");
            return 1;
        }

        int value = atoi(argv[7]);
        handle_update_threshold(role, district, value);
    }

    // ----------------------
    // FILTER
    // ----------------------
    else if (strcmp(command, "--filter") == 0) {
        if (argc < 8) {
            printf("Usage: --filter <district> <conditions>\n");
            return 1;
        }

        handle_filter(role, district, argc, argv, 7);
    }

    // ----------------------
    // INVALID
    // ----------------------
    else {
        printf("Unknown command\n");
    }

    return 0;
}