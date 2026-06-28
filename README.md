# Linux Shell in C++

A custom Unix-like shell implemented in C++ using Linux system calls.

## Overview

This project is a simplified Linux shell that supports execution of external commands, input/output redirection, pipelines, background processes, signal handling, and built-in commands.

The shell is implemented using core Operating System concepts such as process creation, process synchronization, file descriptors, pipes, signals, and system calls.

## Features

### Command Execution

Execute standard Linux commands.

Example:

```bash
myshell> ls
myshell> pwd
myshell> grep main myshell.cpp
```

### Built-in `cd` Command

Change the current working directory.

Example:

```bash
myshell> cd Documents
```

### Output Redirection

Redirect command output to a file.

Example:

```bash
myshell> ls > files.txt
```

### Output Append Redirection

Append command output to an existing file.

Example:

```bash
myshell> echo hello >> log.txt
```

### Input Redirection

Read command input from a file.

Example:

```bash
myshell> cat < data.txt
```

### Pipelines

Connect multiple commands using pipes.

Example:

```bash
myshell> ls | grep cpp
myshell> cat file.txt | grep hello | wc -l
```

### Background Processes

Run commands without blocking the shell.

Example:

```bash
myshell> sleep 10 &
```

### Zombie Process Cleanup

Automatically reaps completed background processes using `waitpid()` with `WNOHANG`.

### Signal Handling

Handles:

* `SIGINT` (Ctrl+C)
* `SIGTSTP` (Ctrl+Z)

The shell remains active while child processes use default signal behavior.

---

## Operating System Concepts Used

* Process Creation (`fork`)
* Program Execution (`execvp`)
* Process Synchronization (`wait`, `waitpid`)
* Pipes (`pipe`)
* File Descriptors
* I/O Redirection (`dup2`)
* Signal Handling (`signal`)
* Background Process Management
* Zombie Process Reaping

---

## Build Instructions

Compile:

```bash
g++ myshell.cpp -o myshell
```

Run:

```bash
./myshell
```

---

## Example Session

```bash
myshell> pwd
/home/user

myshell> ls | grep cpp
myshell.cpp

myshell> echo hello > output.txt

myshell> cat output.txt
hello

myshell> sleep 5 &
```

---

## Project Structure

```text
myshell.cpp    Main source code
```

---

## Learning Outcomes

Through this project I gained hands-on experience with:

* Linux process management
* System calls
* Inter-process communication
* Pipe implementation
* File descriptor manipulation
* Shell command execution flow
* Signal handling in Unix systems

---

