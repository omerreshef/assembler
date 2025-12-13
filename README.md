# Assembler

A small two-pass assembler project written in C.
This Assembler is a project in C language course in the Open University of Israel.
Student name - Omer Reshef.

Here is a short explaination of this program parts:
The assembler has three main parts: preprocessing, first pass and second pass.

## Preprocessing
Dealing with the .as file input.
Removing irrelevant spaces, tabs etc.
Scanning all program macros, creating a new .am file with a convert of the macros to the relevant instructions.

## First Pass
Dealing the the .am file input.
Reading the file content in order to initialize the symbol table and parse the file lines.

## Seconds pass
Scanning again the program lines in order to finalize the symbol tables and encode the lines.
After this step, we can take the function output and create from it all the output files.

## Function output
.ext file - externals file with all the usages of extern labels. (If there are no externs, this file is not created).
.ent file - entries file with all the usages of entry labels. (If there are no entries, this file is not created).
.ob file - object file with the machine code of all the program lines.

## Error handling
If there is some error during the program execution, the error description will be printed to the user screen.

## Build

Required package - gcc.

From the root directory:

```bash
make
```

In order to remove the compiled program:
```bash
make clean
```

## Usage

The assembler executable takes one or more file base names (without extension). For each argument `file` it will read `file.as` and generate `file.ob`, `file.ext`, and `file.ent`.

```bash
./assembler <fileBase1> [fileBase2 ...]
# example:
./assembler myprog another
# reads: myprog.as, another.as
# writes: myprog.ob, myprog.ext, myprog.ent, another.ob, ...
```
