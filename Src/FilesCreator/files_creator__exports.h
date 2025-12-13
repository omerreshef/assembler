#include "common__exports.h"

/**
 * Builds the extern file from the extern usages found during the second pass.
 * @param file_path The path to create the extern file at.
 * @param extern_usages The extern usages found during the second pass.
 * 
 * On success, creates the extern file at file_path and returns SUCCESS.
 */
RC_t FILES_CREATOR__create_ext_file(const char *file_path, extern_usages_t *extern_usages);

/**
 * Builds the entry file from the symbol table created during the first pass.
 * @param file_path The path to create the entry file at.
 * @param symbol_table The symbol table created during the first pass.
 * 
 * On success, creates the entry file at file_path and returns SUCCESS.
 */
RC_t FILES_CREATOR__create_ent_file(const char *file_path, symbol_table_t *symbol_table);

/**
 * Builds the assembly (object) file from the encoded lines created during the second pass.
 * @param file_path The path to create the assembly file at.
 * @param encoded_lines The encoded lines created during the second pass.
 * @param parsed_lines The parsed lines created during the first pass.
 * 
 * On success, creates the assembly file at file_path and returns SUCCESS.
 */
RC_t FILES_CREATOR__create_asm_file(const char *file_path, encoded_lines_t *encoded_lines ,parsed_lines_t *parsed_lines);