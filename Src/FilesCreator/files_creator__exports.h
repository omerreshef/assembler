#include "common__exports.h"


RC_t FILES_CREATOR__create_ext_file(const char *file_path, extern_usages_t *extern_usages);
RC_t FILES_CREATOR__create_ent_file(const char *file_path, symbol_table_t *symbol_table);
RC_t FILES_CREATOR__create_asm_file(const char *file_path, encoded_lines_t *encoded_lines ,parsed_lines_t *parsed_lines);