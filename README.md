# 🏙️ City Manager – OS Project (Phase 1)

## 📌 Overview

This project implements a UNIX-based C application for managing city infrastructure reports (e.g. damaged roads, broken lighting, flooding).

The system allows inspectors to submit reports and managers to monitor, update, and control access to them.

The application simulates a real-world system using:

* low-level file operations (`open`, `read`, `write`, `lseek`)
* file permissions (`chmod`, `stat`)
* symbolic links
* structured binary data

---

## 👥 User Roles

The system supports two roles:

* **Inspector**

  * can add, view, list, and filter reports
* **Manager**

  * full access, including removing reports and updating configuration

Roles are passed via CLI:

```bash
./city_manager --role inspector --user bob --list downtown
```

---

## 📂 Project Structure

```
city_manager/
│
├── src/
│   ├── main.c
│
│   ├── commands/        # CLI command logic
│   ├── file/            # low-level file operations
│   ├── permissions/     # permission handling (stat, chmod)
│   ├── filter/          # condition parsing & matching
│   ├── utils/           # helper functions
│   ├── models/          # data structures (Report)
│
├── Makefile
├── ai_usage.md
├── README.md
```

---

## 🧱 Data Model

Each report is stored as a fixed-size struct in a binary file:

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

---

## 📁 Disk Layout

Each district has its own directory:

```
<district_id>/
├── reports.dat        # binary file (reports)
├── district.cfg       # configuration (severity threshold)
├── logged_district    # operation log
```

Additionally, a symbolic link is created:

```
active_reports-<district_id> -> <district_id>/reports.dat
```

---

## 🔐 File Permissions

| File            | Permissions | Description                          |
| --------------- | ----------- | ------------------------------------ |
| Directory       | 750         | manager full, inspector read/execute |
| reports.dat     | 664         | both can read/write                  |
| district.cfg    | 640         | manager write, inspector read        |
| logged_district | 644         | everyone read, manager write         |

Permissions are:

* explicitly set using `chmod()`
* validated using `stat()` before operations

---

## ⚙️ Implemented / Planned Commands

* `add <district>` → add report
* `list <district>` → list all reports
* `view <district> <id>` → view specific report
* `remove_report <district> <id>` → manager only
* `update_threshold <district> <value>` → manager only
* `filter <district> <conditions>` → filter reports

---

## 🔍 Filter System (AI-assisted)

Conditions format:

```
field:operator:value
```

Examples:

```
severity:>=:2
category:==:road
```

AI is used to generate:

* `parse_condition()`
* `match_condition()`

All integration logic is implemented manually.

---

## 🧠 Key Concepts Used

* Binary file manipulation
* File metadata and permissions
* Symbolic links (`symlink`, `lstat`)
* Process-safe file operations
* Modular C architecture

---

## 🚧 Current Status

* [x] Project structure created
* [ ] Report struct defined
* [ ] Add command
* [ ] List command
* [ ] Remove report logic
* [ ] Filter system
* [ ] Permission validation
* [ ] Symlink handling

---

## 📝 Notes

This project is part of the Operating Systems course.
Each phase builds upon the previous one — failing a phase results in failing the entire project.

---
