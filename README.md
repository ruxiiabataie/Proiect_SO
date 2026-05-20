# City Manager – UNIX Operating Systems Project

## Overview

City Manager is a UNIX-based C application that simulates a city infrastructure issue reporting and monitoring system.

Inspectors can submit infrastructure reports (damaged roads, flooding, broken lighting, etc.), while managers can supervise districts, update configuration files, remove reports, and manage monitoring services.

The project was developed incrementally across three phases focused on:

* file systems
* permissions
* binary file manipulation
* processes and signals
* pipes and I/O redirection
* inter-process communication (IPC)

The application uses low-level UNIX system calls extensively.

---

# Phase Structure

## Phase 1 – File Systems

Implemented:

* binary report storage
* district directories
* configuration files
* operation logs
* symbolic links
* permission management
* report filtering

System calls used:

* open
* read
* write
* lseek
* ftruncate
* stat
* lstat
* mkdir
* chmod
* symlink
* unlink

---

## Phase 2 – Processes and Signals

Implemented:

* district removal using child processes + exec
* monitor process (`monitor_reports`)
* PID file management
* signal handling (`SIGUSR1`, `SIGINT`)
* monitor notification when new reports are added

System calls used:

* fork
* exec
* kill
* sigaction
* unlink

---

## Phase 3 – Pipes and Redirects

Implemented:

* interactive `city_hub`
* monitor launching through pipes
* IPC using `pipe()`
* output redirection with `dup2()`
* multiple scorer processes
* workload score computation per inspector

System calls used:

* pipe
* dup2
* fork
* exec
* read
* write

---

# User Roles

The application supports two roles.

## Inspector

Can:

* add reports
* list reports
* view reports
* filter reports

## Manager

Has full access:

* remove reports
* update thresholds
* remove districts
* manage monitoring operations

Example:

```bash
./city_manager --role inspector --user bob --list downtown
```

---

# Project Structure

```text
CITY_MANAGER/
│
├── src/
│   ├── main.c
│   ├── city_hub.c
│   ├── monitor_reports.c
│   ├── scorer.c
│   │
│   ├── commands/
│   ├── file/
│   ├── filter/
│   ├── permissions/
│   ├── models/
│
├── downtown/
├── uptown/
│
├── active_reports-downtown
├── active_reports-uptown
│
├── Makefile
├── README.md
├── AI_usage-ALL-phases.md
│
├── city_manager
├── city_hub
├── monitor_reports
├── scorer
```

---

# Report Structure

Each report is stored as a fixed-size binary struct:

```c
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
```

Reports are stored sequentially in:

```text
reports.dat
```

using binary I/O.

---

# District Layout

Each district has its own directory:

```text
<district>/
├── reports.dat
├── district.cfg
├── logged_district
```

Additionally, symbolic links are created:

```text
active_reports-<district> -> <district>/reports.dat
```

---

# Permission System

The program explicitly sets and validates UNIX permission bits.

| File               | Permissions | Purpose                   |
| ------------------ | ----------- | ------------------------- |
| district directory | 750         | manager full access       |
| reports.dat        | 664         | both roles may read/write |
| district.cfg       | 640         | manager write only        |
| logged_district    | 644         | everyone read             |

Permissions are checked using:

* stat()
* st_mode
* permission macros

---

# Implemented Commands

## city_manager

### Add report

```bash
./city_manager --role inspector --user alice --add downtown
```

---

### List reports

```bash
./city_manager --role inspector --user bob --list downtown
```

Displays:

* all reports
* file permissions
* file size
* modification time

---

### View report

```bash
./city_manager --role inspector --user bob --view downtown 17
```

---

### Remove report

```bash
./city_manager --role manager --user alice --remove_report downtown 17
```

Uses:

* lseek()
* ftruncate()

to shift records and shrink the binary file.

---

### Update threshold

```bash
./city_manager --role manager --user alice --update_threshold downtown 3
```

---

### Filter reports

```bash
./city_manager --role inspector --user bob --filter downtown severity:>=:2
```

Supported fields:

* severity
* category
* inspector
* timestamp

Supported operators:

* ==
* !=
* <
* <=
* >
* > =

---

### Remove district

```bash
./city_manager --role manager --user alice --remove_district uptown
```

Implemented using:

* fork()
* exec()
* external rm -rf

---

# monitor_reports

The monitor process:

* creates `.monitor_pid`
* waits for signals
* reacts to `SIGUSR1`
* ends only on `SIGINT`

Example output:

```text
Monitor started. PID: 12345
New report added!
Monitor shutting down...
```

---

# city_hub

`city_hub` provides an interactive shell interface.

Run:

```bash
./city_hub
```

---

# Supported Commands

## Start monitor

```text
start_monitor
```

Creates:

* child processes
* pipes
* redirected monitor output

---

## Stop monitor

```text
stop_monitor
```

Sends:

* SIGINT

to the monitor process.

---

## Calculate scores

```text
calculate_scores downtown uptown
```

Creates:

* one scorer process per district
* one pipe per scorer

Each scorer computes:

```text
workload score = sum of severity levels
```

per inspector.

Example:

```text
District: downtown

alice -> 2
bob -> 8
```

---

## Exit

```text
exit
```

---

# Inter-Process Communication

The project uses:

* pipe()
* dup2()
* fork()
* exec()

for communication between:

* city_hub
* monitor_reports
* scorer

---

# AI-Assisted Components

AI assistance was used for:

* filter condition parsing
* filter condition matching
* debugging IPC
* signal handling debugging
* process management debugging

All AI interactions and modifications are documented in:

```text
AI_usage-ALL-phases.md
```

---

# Build Instructions

Compile all executables:

```bash
make
```

Clean executables:

```bash
make clean
```

---

# Running the Programs

## city_manager

```bash
./city_manager --role inspector --user alice --add downtown
```

---

## city_hub

```bash
./city_hub
```

---

# Example city_hub Session

```text
city_hub> start_monitor

FROM MONITOR: Monitor started. PID: 97233

Monitor launched successfully

city_hub> calculate_scores downtown uptown

Processing district: downtown

alice -> 2
bob -> 8

Processing district: uptown

alice -> 3
bob -> 5

city_hub> stop_monitor

Monitor stopped successfully
```

---

# Concepts Used

* binary file management
* UNIX permissions
* symbolic links
* process creation
* signal handling
* pipes
* stdout redirection
* inter-process communication
* fixed-size record storage
* low-level UNIX system calls

---

# Deliverables Included

* city_manager
* monitor_reports
* city_hub
* scorer
* district directories
* symlinks
* operation logs
* Makefile
* AI usage documentation

---

# Notes

This project was developed for the Operating Systems course and follows all three required phases from the official specification.
