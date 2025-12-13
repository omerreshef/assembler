#pragma once

#include "first_pass__exports.h"

#define INITIAL_IC_ADDRESS 100
#define INITIAL_DC_ADDRESS 0

/**
 * Scanning the symbol table and detecting multiple defintion of the same symbol name.
 * @param symbol_name all the loaded symbols to be checked
 * @param symbols_amount amount of symbols to check in the array (the rest are empty).
 * On success, if a duplication was found, return FIRST_PASS__DETECT_SYMBOLS_DUPLICATION__FOUND_DUPLICATION.
 * If no duplication was found, return SUCCESS.
 */
RC_t first_pass__detect_symbols_duplication(symbol_table_t *symbol_table, int symbols_amount);

/**
 * Scanning the entries list and detecting multiple defintion of the same entry name.
 * @param entries_list all the loaded entries to be checked
 * @param entries_amount amount of entries to check in the array (the rest are empty).
 * On success, if a duplication was found, return FIRST_PASS__DETECT_ENTRIES_DUPLICATION__FOUND_DUPLICATION.
 * If no duplication was found, return SUCCESS.
 */
RC_t first_pass__detect_entries_duplication(program_entries_t *entries_list, int entries_amount);

/**
 * Allocate memory for symbol_table and entries_list according to the parsed lines.
 * @param parsed_lines all the parsed lines from the input file.
 * @param symbol_table table to store all the scanned symbols of the first pass.
 * @param entries_list array to store all the scanned symbols of the first pass.
 * @param encoded_lines array to store all the encoded lines.
 * @param lines_amount amount of lines to allocate in encoded_lines.
 * On success, allocate memory and return SUCCESS.
 */
RC_t first_pass__allocate_structures_memory(parsed_lines_t *parsed_lines, symbol_table_t *symbol_table, program_entries_t *entries_list, encoded_lines_t *encoded_lines, int lines_amount);
