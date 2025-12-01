#pragma once

#include "common__exports.h"

#define NULL_TERMINATOR_SIZE 1

typedef enum line_type_t {
    LINE_TYPE__UNINITIALIZED = 0,
    LINE_TYPE__EMPTY,
    LINE_TYPE__INSTRUCTION,
    LINE_TYPE__DATA,
    LINE_TYPE__STRING,
    LINE_TYPE__EXTERN,
    LINE_TYPE__ENTRY
} line_type_t;

typedef struct parsed_line_t {
    line_type_t line_type;
    int *numbers;
    int numbers_count;
    int line_ic;
    char *label;
    char *entry_name;
    char *extern_name;
    char *string;
    char *opcode;
    char *operands[2];
} parsed_line_t;

/**
 * Parses a line of assembly code into its components.
 * @param line_buffer The line of assembly code to parse.
 * @param parsed_line Pointer to the parsed_line_t structure to store the parsed components.
 * 
 * On success, the parsed components are allocated and stored in parsed_line and return SUCCESS.
 */
RC_t LINE_PARSER__parse_line(const char *line_buffer, parsed_line_t *parsed_line);

