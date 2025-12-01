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
RC_t first_pass__detect_entries_duplication(char *entries_list[MAX_ENTRIES], int entries_amount);
