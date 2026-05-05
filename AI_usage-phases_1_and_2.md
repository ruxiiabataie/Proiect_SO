For the filter command, I used ChatGPT only for the two functions required in the project: parse_condition() and match_condition().
The project is a C program called city_manager, where reports about city problems like damaged roads, broken lights or flooding are stored in a binary file called reports.dat.
The filter command is used to search reports using conditions like:
severity:>=:2
category:==:road
inspector:!=:alice
For the first function, parse_condition(), I asked ChatGPT for a function that takes a string like severity:>=:2 and splits it into 3 parts: field, operator and value.
It generated this:
int parse_condition(const char *input, char *field, char *op, char *value) {
    return sscanf(input, "%[^:]:%[^:]:%s", field, op, value);
}
I kept it exactly like that because it was simple and correct. I tested it with a few examples and it worked without problems.
The second function was match_condition().
Here I asked for a function that checks if a report matches a condition depending on the field (severity, category, inspector, timestamp) and the operator (==, !=, <, >, <=, >=).
It also generated a helper function called match_int() for comparing numbers.
For example, if the condition is:
severity:>=:2
the function checks if the severity from the report is greater than or equal to 2.
If the condition is:
category:==:road
it compares the category from the report with "road" using strcmp().
For inspector, it works the same way, and for timestamp, it converts the value to integer and compares it as a number.
I kept most of this code too because it was already good, but I checked it line by line to make sure I really understood it.
For example:
atoi() changes values like "2" from text into integers
strcmp() is used for comparing strings like category and inspector
match_int() makes the numeric comparisons cleaner and avoids repeating code
I tested the filter command with real examples like:
severity:>=:2
category:==:road
and checked that only the correct reports were displayed.

## Phase 2

For Phase 2, I used ChatGPT mainly to understand how processes and signals work in C, and how to connect two separate programs.

---

### 1. remove_district

For this part, I needed to delete an entire directory using a child process and the external command `rm -rf`.

I asked ChatGPT how to correctly use `fork()` and `exec()` to run a command from a C program.

It suggested using:

* `fork()` to create a child process
* `execlp()` to execute the command
* `wait()` so the parent waits for the child

The generated idea was:

```c
pid_t pid = fork();

if (pid == 0) {
    execlp("rm", "rm", "-rf", district, NULL);
}
else {
    wait(NULL);
}
```

I kept this logic, but I added:

* error handling for `fork()` and `exec()`
* deletion of the symbolic link using `unlink()` before removing the directory

I tested it by creating districts and then deleting them, and I verified with `ls` that both the directory and the symlink were removed.

---

### 2. monitor_reports program

Here I had to write a separate program that reacts to signals.

I asked ChatGPT how to:

* create a process that runs continuously
* handle signals using `sigaction()`
* safely print messages inside a signal handler

It suggested using:

* `sigaction()` instead of `signal()`
* `pause()` to wait for signals
* `write()` instead of `printf()` inside handlers

I implemented:

* writing the PID into a file called `.monitor_pid` using `getpid()`
* a handler for `SIGUSR1` that prints "New report added!"
* a handler for `SIGINT` (Ctrl+C) that deletes `.monitor_pid` and exits

Example:

```c
void handle_sigusr1(int sig) {
    write(STDOUT_FILENO, "New report added!\n", 18);
}
```

I kept the logic but made sure I understood why `write()` is used instead of `printf()` (because it is safe inside signal handlers).

---

### 3. Notifying the monitor (kill + PID file)

The last step was connecting the two programs.

I asked ChatGPT how one process can notify another using signals.

The solution was:

* the monitor writes its PID into `.monitor_pid`
* the main program reads that PID
* it sends a signal using `kill(pid, SIGUSR1)`

I implemented a function like this:

```c
int notify_monitor() {
    int fd = open(".monitor_pid", O_RDONLY);
    if (fd < 0) return -1;

    char buffer[32];
    int n = read(fd, buffer, sizeof(buffer) - 1);
    close(fd);

    if (n <= 0) return -1;

    buffer[n] = '\0';
    pid_t pid = atoi(buffer);

    if (kill(pid, SIGUSR1) < 0)
        return -1;

    return 0;
}
```

Then I called this function inside `handle_add()` after adding a report.

If the notification worked, I logged:

```
Monitor notified successfully
```

If not, I logged:

```
Monitor could not be informed
```

---

### What I learned in Phase 2

* how processes are created with `fork()`
* how `exec()` replaces a process with a new program
* how signals work in UNIX
* how processes communicate using `kill()`
* how to safely handle signals using `sigaction()`
* how to debug problems with file paths (for `.monitor_pid`)

---

### Testing

I tested everything by:

* running `monitor_reports` in one terminal
* running `city_manager --add` in another
* checking that the monitor prints "New report added!"
* checking logs for success and failure cases
* testing behavior when the monitor is not running

All features worked as expected.
