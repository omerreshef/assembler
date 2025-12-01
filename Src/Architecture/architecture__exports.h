#pragma once
#include <stdbool.h>

#define MAX_OPCODE_LEN 5

/**
 * Check if the given name is a valid instruction.
 * @param name The name to check.
 * @param is_instruction Pointer to a boolean that will be set to true if the name is a valid instruction, false otherwise.
 * On success, sets *is_instruction accordingly and returns SUCCESS.
 */
RC_t ARCHITECTURE__is_instruction(char *name, bool *is_instruction);

/**
 * Check if the given name is a valid opcode.
 * @param name The name to check.
 * @param is_instruction Pointer to a boolean that will be set to true if the name is a valid opcode, false otherwise.
 * On success, sets *is_opcode accordingly and returns SUCCESS.
 */
RC_t ARCHITECTURE__is_opcode(char *name, bool *is_opcode);


/**
 * Check if the given name is a valid register.
 * @param name The name to check.
 * @param is_instruction Pointer to a boolean that will be set to true if the name is a valid register, false otherwise.
 * On success, sets *is_register accordingly and returns SUCCESS.
 */
RC_t ARCHITECTURE__is_register(char *name, bool *is_register);


/**
 * Get the register value on assembly machine code.
 * @param name The register name.
 * @param register_value Pointer to a integer that the integer value will be saved in.
 * On success, sets *register_value and returns SUCCESS;
 */
RC_t ARCHITECTURE__get_register_value(char *name, int *register_value);

/** 
 * Get the operands number of a given opcode
 * @param opcode_name The name of the opcode.
 * @param opcode_size Pointer to an integer that will be set to the operands number.
 * On success, sets *operands_number and returns SUCCESS.
 */
RC_t ARCHITECTURE__get_opcode_operands_number(char *opcode_name, int *operands_number);

/**
 * Get the size (in words) of the given opcode.
 * @param opcode_name The name of the opcode.
 * @param opcode_size Pointer to an integer that will be set to the size of the opcode.
 * On success, sets *opcode_size and returns SUCCESS.
 */
RC_t ARCHITECTURE__get_opcode_size(char *opcode_name, int *opcode_size);
