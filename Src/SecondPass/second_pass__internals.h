#pragma once

#include "common__exports.h"
#include "assembler__exports.h"

#define MAX_EXTERN_USAGES 1000

#define MAKE_MACHINE_CODE(opcode, funct, source_address_mode, destination_address_mode) \
( ( (opcode)                   & 0xF ) << 8 ) | \
( ( (funct)                    & 0xF ) << 4 ) | \
( ( (source_address_mode)      & 0x3 ) << 2 ) | \
( ( (destination_address_mode) & 0x3 ) << 0 )


/**
 * Finalizes the symbol table by marking entries.
 * @param symbol_table The symbol table to finalize.
 * @param entries_list The list of entry names to mark in the symbol table.
 * 
 * On success, marks the entries in the symbol table and returns SUCCESS.
 * If an entry is not found or is marked as external, returns an appropriate error code.
 */
RC_t second_pass__finalize_symbol_table(symbol_table_t *symbol_table, program_entries_t *entries_list);

/**
 * Records a new extern usage.
 * @param extern_usages The extern usages structure to record the usage.
 * @param operand_ic The instruction counter value where the extern is used.
 * @param symbol_name The name of the extern symbol being used.
 * 
 * On success, records the extern usage and returns SUCCESS.
 */
RC_t second_pass__enter_new_extern_usage(extern_usages_t *extern_usages, int operand_ic, char *symbol_name);

/**
 * Resolves an operand to its value and type.
 * @param operand The operand string to resolve.
 * @param symbol_table The symbol table for resolving symbols.
 * @param operand_ic The instruction counter value of the operand.
 * @param operand_value Pointer to store the resolved operand value.
 * @param operand_type Pointer to store the resolved operand type.
 * @param extern_usages The extern usages structure to record any extern references.
 * 
 * On success, fills operand_value and operand_type with the resolved values and returns SUCCESS.
 */
RC_t second_pass__resolve_operand(char *operand, symbol_table_t *symbol_table, int operand_ic, int *operand_value, operand_type_t *operand_type, extern_usages_t *extern_usages);

/**
 * Encoding of a single instruction operand.
 * @param operand_value The value of the operand to encode.
 * @param operand_type The addressing type of the operand.
 * @param encoded_operand Pointer to store the encoded operand value.
 */
RC_t second_pass__handle_instruction_operand(int operand_value, operand_type_t operand_type, int *encoded_operand);

/**
 * Encodes an instruction line into machine code.
 * @param instruction The parsed instruction line.
 * @param symbol_table The symbol table for resolving symbols.
 * @param first_operand_value The resolved value of the first operand.
 * @param second_operand_value The resolved value of the second operand.
 * @param first_operand_type The addressing type of the first operand.
 * @param second_operand_type The addressing type of the second operand.
 * @param encoded_instruction Array to store the encoded instruction words.
 * @param words_count Pointer to store the number of words used in the encoded instruction.
 * 
 * On success, fills encoded_instruction with the machine code and sets words_count, returning SUCCESS.
 */
RC_t second_pass__encode_instruction(parsed_line_t *instruction,
    symbol_table_t *symbol_table,
    int first_operand_value,
    int second_operand_value,
    operand_type_t first_operand_type,
    operand_type_t second_operand_type,
    int encoded_instruction[MAX_WORDS_IN_INSTRUCTION],
    int *words_count);

/**
 * Encodes a single line based on its parsed representation.
 * @param extern_usages param to store all the extern usages found during encoding.
 * @param parsed_line The parsed representation of the line to encode.
 * @param symbol_table The symbol table for resolving symbols.
 * @param encoded_line Pointer to the encoded_line_t structure to store the encoded line.
 * 
 * On success, the encoded line is allocated and stored in encoded_line and return SUCCESS.
 * If extern usages are found, they are stored in extern_usages.
 */
RC_t second_pass__encode_line(extern_usages_t *extern_usages, parsed_line_t *parsed_line, symbol_table_t *symbol_table, encoded_line_t *encoded_line);
