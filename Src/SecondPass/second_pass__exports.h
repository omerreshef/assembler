#define MAX_WORDS_IN_INSTRUCTION 3

/**
 * Preform the second pass on input file, after the first pass part.
 * @param encoded_lines param to store the final encoded lines of the program.
 * @param extern_usages param to store all the extern usages found during the second pass.
 * @param parsed_lines array containing details of every assembly line.
 * @param symbol_table table containing all the scanned symbols of the first pass.
 * @param entries_list array containing all the scanned entries of the first pass.
 * 
 * On success, stores the encoded lines in encoded_lines, stores the extern usages in extern_usages, and returns SUCCESS.
 */
RC_t SECOND_PASS__process(program_encoded_lines_t *encoded_lines, extern_usages_t *extern_usages, parsed_lines_t *parsed_lines, symbol_table_t *symbol_table, program_entries_t *entries_list);