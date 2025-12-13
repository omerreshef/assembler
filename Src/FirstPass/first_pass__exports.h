#pragma once

#include "common__exports.h"
#include "line_parser__exports.h"
#include "assembler__exports.h"

/**
 * Preform the first pass on input file, after the pre-assembler part.
 * @param input_file_path assembly file to perform the file pass on.
 * @param instruction_counter param to store the final value of instruction_counter.
 * @param data_counter param to store the final value of data_counter.
 * @param parsed_lines array to store details of every assembly line.
 * @param symbol_table table to store all the scanned symbols of the first pass.
 * @param entries_list array to store all the scanned symbols of the first pass. 
 */
RC_t FIRST_PASS__process(char *input_file_path, int *instruction_counter, int *data_counter, parsed_lines_t *parsed_lines, symbol_table_t *symbol_table, program_entries_t *entries_list);
