#pragma once

#include "line_parser__exports.h"
#include "common__exports.h"

#define LABEL_SKIP_LENGTH 2
#define SPACE_CHARACTER_LEN 1
#define QUOTATION_CHARACTER_LEN 1

/** 
 * Parses instruction operands from a given instruction line.
 * @param instruction The instruction mnemonic.
 * @param arguments The arguments string containing operands.
 * @param parsed_line Pointer to the parsed_line_t structure to store the parsed operands.
 * 
 * On success, the operands are allocated and stored in parsed_line->operands and return SUCCESS.
 */
RC_t line_parser__parse_instruction_operands(const char* instruction, const char* arguments, parsed_line_t *parsed_line);

/**
 * Parses data numbers from a .data line.
 * @param data The data string containing comma-separated numbers.
 * @param parsed_line Pointer to the parsed_line_t structure to store the parsed numbers.
 * 
 * On success, the numbers are allocated and stored in parsed_line->numbers and return SUCCESS.
 */
RC_t line_parser__parse_data_numbers(char* data, parsed_line_t *parsed_line);
