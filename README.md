[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/J7xkl66X)

# myshell

A simple UNIX-like shell implemented in C++ that supports basic commands, I/O redirection, pipelines, background processes, and more.

---

## Features

- Execute built-in commands (`cd`, `pwd`, `exit`)
- Run external commands (e.g., `ls`, `cat`, `grep`)
- Handle **input (`<`)** and **output (`>`, `>>`) redirection**
- Support **pipes (`|`)** between commands
- Run **background jobs** using `&`
- Display useful error messages for invalid commands

---
---

## Project Structure

```bash
myshell/
├── command.cc # Command execution and logic
├── command.h # Command class and function declarations
├── tokenizer.cc # Token parsing and command splitting
├── tokenizer.h # Tokenizer class and helpers
└── Makefile # Build configuration
```

---

## Build Instructions

To compile the shell manually:

```bash
g++ -std=gnu++17 -O2 -Wall -Wextra -Wno-unused-parameter command.cc tokenizer.cc -o myshell
```

Or simply use the provided Makefile:
```bash
make
```

---
---
## Usage

You can compile and run with:
```bash
make run
```

Or only run the shell with:
```bash
./myshell
```
You can then enter commands like:
```bash
ls -al
echo "Hello OS Lab"
cat input.txt > output.txt
grep "line" log.txt | wc -l
sleep 2 &
```

---
---

## Cleaning Up
To remove compiled files:
```bash
make clean
```