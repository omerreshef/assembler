#pragma once
#include "common__exports.h"

#define MAX_MCRO_NAME_LENGTH 100
#define MAX_MCROS_COUNT 100
#define MAX_MCRO_LENGTH 1000

typedef struct mcro_t {
    char name[MAX_MCRO_NAME_LENGTH];
    char body[MAX_MCRO_LENGTH];
} mcro_t;

/**
 * Reads assembly file, and convert all the mcro definitions to the actual
 * code behind it.
 * @input_file_path: file to read the assembly code from
 * @output_file_path: file to write the converted assembly 
 * 
 * On success, write the new converted file to output_file_path and returns SUCCESS.
 */
RC_t MCRO_READER__convert_mcros_to_instructions(const char *input_file_path, const char *output_file_path);