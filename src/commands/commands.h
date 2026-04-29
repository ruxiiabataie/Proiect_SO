#ifndef COMMANDS_H
#define COMMANDS_H

// ADD
void handle_add(const char *role, const char *user, const char *district);

// LIST
void handle_list(const char *role, const char *district);

// VIEW
void handle_view(const char *role, const char *district, int report_id);

// REMOVE (manager only)
void handle_remove(const char *role, const char *district, int report_id);

// UPDATE THRESHOLD (manager only)
void handle_update_threshold(const char *role, const char *district, int value);

// FILTER
void handle_filter(const char *role, const char *district, int argc, char *argv[], int start_index);

#endif