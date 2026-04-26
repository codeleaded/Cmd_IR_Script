# Project README

## Overview
This project is a collection of C/C++ libraries and scripts that support the development of custom languages. The primary focus is on creating a simple scripting environment where users can define their own types, functions, and variables with specific behaviors defined through custom operators.

## Features
- **Custom Types**: Users can define new types with specific operator overloads.
- **Variable Assignment**: Support for assigning values to variables using custom assignment operators.
- **Function Pointers**: Allow storing function pointers in variables and use them later.
- **Memory Management**: Custom allocators and destructors for dynamically allocated data.

## Project Structure
```
<Project>/
├── build/              # .exe files produced by Main.c
├── bin/                # .so / .dll produced by *.c in libs
├── libs/               # *.c for bin
│   ├── func.c          # C code for handling function pointers
│   ├── var.c           # C code for handling variables
│   └── type.c          # C code for defining and managing types
├── lib/                # librarys for my own languages
├── code/               # scripts from my custom languages for example .rex, .ll, .omml
├── data/               # Datafile for example .txt or dumped files
├── assets/             # images and sound files
├── src/                # src code
│   ├── Main.c          # Entry point
│   └── *.h             # stand alone Header-based C-files, without *.c files that implement it
├── Makefile.linux      # Linux Build configuration
├── Makefile.windows    # Windows Build configuration
├── Makefile.wine       # Wine Build configuration
└── README.md           # This file
```

### Prerequisites
- C/C++ Compiler and Debugger (GCC, Clang)
- Make utility
- Standard development tools

## Build & Run
To build the project, use one of the provided makefiles depending on your operating system. Here are the steps for building on Linux:

1. Navigate to the project directory:
   ```sh
   cd <Project>
   ```

2. Build the project using Makefile.linux:
   ```sh
   make -f Makefile.linux all
   ```

3. To execute the built application, run:
   ```sh
   make -f Makefile.linux exe
   ```

For a clean rebuild and execution on Linux, you can use:
```sh
make -f Makefile.linux clean
make -f Makefile.linux all
make -f Makefile.linux exe
```

The same process applies to Windows and other platforms with the appropriate makefile.