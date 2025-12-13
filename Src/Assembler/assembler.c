#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#include "second_pass__exports.h"
#include "text_editor__exports.h"
#include "common__exports.h"
#include "mcro_reader__exports.h"
#include "line_parser__exports.h"
#include "files_creator__exports.h"
#include "file__exports.h"
#include "memory_cleaner__exports.h"
#include "first_pass__exports.h"

#include "assembler__internals.h"


int main(int argc, char *argv[])
{
    RC_t return_code = UNINITIALIZED;
    bool is_file_exists = false;
    int instruction_counter = 0;
    int data_counter = 100;
    int base_len = 0;
    int i = 0;
    char *current_arg = NULL;
    char *input_as = NULL;
    char *temp_file = NULL;
    char *am_file = NULL;
    char *ent_file = NULL;
    char *ext_file = NULL;
    char *ob_file = NULL;
    parsed_lines_t parsed_lines = {0};
    symbol_table_t symbol_table = {0};
    program_entries_t entries_list = {0};
    extern_usages_t extern_usages = {0};
    encoded_lines_t encoded_lines = {0};

    if (argc < 2) {
        printf("No input files specified. Usage: %s <fileBase1> [fileBase2 ...]\n", argv[0]);
        return_code = SUCCESS;
        goto Exit;
    }

    for (i = 1; i < argc; ++i) {
        current_arg = argv[i];
        base_len = strlen(current_arg);

        input_as = (char *)malloc(base_len + strlen(FILE_EXTENSTION_ASM) + NULL_TERMINATOR_SIZE);
        temp_file = (char *)malloc(base_len + strlen(TEMP_FILE_EXTENSTION) + NULL_TERMINATOR_SIZE);
        am_file = (char *)malloc(base_len + strlen(FILE_EXTENSTION_AM) + NULL_TERMINATOR_SIZE);
        ent_file = (char *)malloc(base_len + strlen(FILE_EXTENSTION_ENT) + NULL_TERMINATOR_SIZE);
        ext_file = (char *)malloc(base_len + strlen(FILE_EXTENSTION_EXT) + NULL_TERMINATOR_SIZE);
        ob_file = (char *)malloc(base_len + strlen(FILE_EXTENSTION_OB) + NULL_TERMINATOR_SIZE);

        if (!input_as || !am_file || !ent_file || !ext_file || !ob_file) {
            fprintf(stderr, "Memory allocation failed for file buffers\n");
            return_code = ASSEMBLER__ALLOCATION_FAILED;
            goto Exit;
        }

        sprintf(input_as, "%s.as", current_arg);
        sprintf(temp_file, "%s.tmp", current_arg);
        sprintf(am_file, "%s.am", current_arg);
        sprintf(ent_file, "%s.ent", current_arg);
        sprintf(ext_file, "%s.ext", current_arg);
        sprintf(ob_file, "%s.ob", current_arg);

        printf("Processing argument %d: input='%s' -> outputs: '%s','%s','%s','%s'\n", i, input_as, am_file, ob_file, ext_file, ent_file);

        EXIT_ON_ERROR(FILE__is_exists(input_as, &is_file_exists), &return_code);
        if (!is_file_exists) {
            fprintf(stderr, "Input file '%s' does not exist.\n", input_as);
            goto Exit;
        }

        /* initialize per-file state */
        instruction_counter = 0;
        data_counter = 100;
        memset(&parsed_lines, 0, sizeof(parsed_lines));
        memset(&symbol_table, 0, sizeof(symbol_table));
        memset(&entries_list, 0, sizeof(entries_list));
        memset(&extern_usages, 0, sizeof(extern_usages));
        memset(&encoded_lines, 0, sizeof(encoded_lines));

        return_code = TEXT_EDITOR__remove_spaces_and_tabs_in_given_path(input_as, temp_file);
        if (return_code != SUCCESS) {
            fprintf(stderr, "Failed to remove spaces and tabs from '%s'\n", input_as);
            goto Exit;
        }

        return_code = MCRO_READER__convert_mcros_to_instructions(temp_file, am_file);
        if (return_code != SUCCESS) {
            fprintf(stderr, "Failed the preprocessing part.\n");
            goto Exit;
        }

        EXIT_ON_ERROR(FILE__delete(temp_file), &return_code);

        return_code = FIRST_PASS__process(am_file, &instruction_counter, &data_counter, &parsed_lines, &symbol_table, &entries_list);
        if (return_code != SUCCESS) {
            fprintf(stderr, "Failed the first pass.\n");
            goto Exit;
        }

        return_code = SECOND_PASS__process(&parsed_lines, &symbol_table, &entries_list, &extern_usages, &encoded_lines);
        if (return_code != SUCCESS) {
            fprintf(stderr, "Failed the second pass.\n");
            goto Exit;
        }

        return_code = FILES_CREATOR__create_ent_file(ent_file, &symbol_table);
        /* There is a chance that there are no entries to write so the file is not created */
        if (return_code != SUCCESS && return_code != FILES_CREATOR__NO_ENTRIES_TO_WRITE) {
            fprintf(stderr, "Failed creating ent file.\n");
            goto Exit;
        }

        return_code = FILES_CREATOR__create_ext_file(ext_file, &extern_usages);
        /* There is a chance that there are no extern usages to write so the file is not created */
        if (return_code != SUCCESS && return_code != FILES_CREATOR__NO_EXTERN_USAGES_TO_WRITE) {
            fprintf(stderr, "Failed creating ext file.\n");
            goto Exit;
        }

        return_code = FILES_CREATOR__create_asm_file(ob_file, &encoded_lines, &parsed_lines);
        if (return_code != SUCCESS) {
            fprintf(stderr, "Failed creating ob file.\n");
            goto Exit;
        }

        /* free per-file allocated memory for parsed/encoded structures */
        (void)MEMORY_CLEANER__clean_allocated_memory(&parsed_lines, &encoded_lines);

        /* free filename buffers */
        free(input_as);
        free(temp_file);
        free(am_file);
        free(ent_file);
        free(ext_file);
        free(ob_file);

        if (return_code != UNINITIALIZED && return_code != SUCCESS) {
            /* if an error happened during processing this argument, stop further processing */
            goto Exit;
        }
    }

    return_code = SUCCESS;
Exit:

    if (return_code != SUCCESS) {
        fprintf(stderr, "Assembly failed with error code: %d, description: %s\n", return_code, RC_description(return_code));
    }

    if (input_as != NULL) {
        free(input_as);
    }
    if (temp_file != NULL) {
        free(temp_file);
    }
    if (am_file != NULL) {
        free(am_file);
    }
    if (ent_file != NULL) {
        free(ent_file);
    }
    if (ext_file != NULL) {
        free(ext_file);
    }
    if (ob_file != NULL) {
        free(ob_file);
    }

    (void)MEMORY_CLEANER__clean_allocated_memory(&parsed_lines, &encoded_lines);

    return return_code;
}
