#pragma once
#include "common__exports.h"

#define MCRO_START_DEFINITION "mcro"
#define MCRO_END_DEFINITION "mcroend"

/**
 * Extracts the mcro name from a given mcro definition line.
 * @line_buffer: line with the mcro definition, to get the mcro name from
 * @mcro_name: destination address to store the mcro name. in
 * 
 * On success, fills the mcro name in mcro_name and returns SUCCESS.
 */
RC_t mcro_reader__extract_mcro_name(const char *line_buffer, char *mcro_name);

/**
 * Find macros amount in a given assembly file.
 * @file_path: path to the file containing macro definitions.
 * @mcros_amount: pointer to store the mcros amount at.
 */
RC_t mcro_reader__count_macros_amount(const char *file_path, int *mcros_amount);

/**
 * Read macros from the specified file and store them in the provided mcros array.
 * @file_path: path to the file containing macro definitions.
 * @mcros: object where the read macros will be stored.
 * 
 * On success, fills the mcros array with the read macros and returns SUCCESS.
 */
RC_t mcro_reader__read_mcros_from_file(const char *file_path, mcros_collection_t *mcros);