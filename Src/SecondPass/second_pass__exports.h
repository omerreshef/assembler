#pragma once

#include "common__exports.h"
#include "assembler__exports.h"
#include "line_parser__exports.h"

#define MAX_WORDS_IN_INSTRUCTION 3
#define MAX_EXTERN_USAGES 1000

typedef struct encoded_line_t {
    int *encoded_line;
    char *words_type;
    int words_count;
} encoded_line_t;

typedef struct encoded_lines_t {
    encoded_line_t encoded_line[MAX_LINES_IN_FILE];
} encoded_lines_t;

typedef struct extern_usage_t {
    char *name;
    int ic;
} extern_usage_t;

typedef struct extern_usages_t {
    extern_usage_t extern_usage[MAX_EXTERN_USAGES];
} extern_usages_t;

/**
 * Preform the second pass on input file, after the first pass part.
 * @param parsed_lines array containing details of every assembly line.
 * @param symbol_table table containing all the scanned symbols of the first pass.
 * @param entries_list array containing all the scanned entries of the first pass.
 * @param extern_usages param to store all the extern usages found during the second pass.
 * @param encoded_lines param to store the final encoded lines of the program.
 * 
 * On success, stores the encoded lines in encoded_lines, stores the extern usages in extern_usages, and returns SUCCESS.
 */
RC_t SECOND_PASS__process(parsed_lines_t *parsed_lines, symbol_table_t *symbol_table, program_entries_t *entries_list,extern_usages_t *extern_usages, encoded_lines_t *encoded_lines);