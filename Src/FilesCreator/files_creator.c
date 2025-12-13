#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "second_pass__exports.h"
#include "files_creator__exports.h"
#include "line_parser__exports.h"
#include "files_creator__internals.h"

RC_t FILES_CREATOR__create_ext_file(const char *file_path, extern_usages_t *extern_usages)
{
    RC_t return_code = UNINITIALIZED;
    FILE *ext_file = NULL;
    int total_extern_usages = 0;
    int i = 0;

    if (file_path == NULL || extern_usages == NULL)
    {
        return_code = FILES_CREATOR__CREATE_EXT_FILE__NULL_ARGUMENT;
        goto Exit;
    }

    for (i = 0; i < MAX_EXTERN_USAGES; i++)
    {
        if (extern_usages->extern_usage[i].name != NULL)
        {
            total_extern_usages++;
        }
    }

    if (total_extern_usages == 0)
    {
        return_code = FILES_CREATOR__NO_EXTERN_USAGES_TO_WRITE;
        goto Exit;
    }

    ext_file = fopen(file_path, "w");
    if (ext_file == NULL)
    {
        return_code = FILES_CREATOR__CREATE_EXT_FILE__FOPEN_ERROR;
        goto Exit;
    }

    for (i = 0; i < MAX_EXTERN_USAGES; i++)
    {
        if (extern_usages->extern_usage[i].name != NULL)
        {
            fprintf(ext_file, "%s %04d\n", extern_usages->extern_usage[i].name, extern_usages->extern_usage[i].ic);
        }
    }

    return_code = SUCCESS;
Exit:
    if (ext_file != NULL)
    {
        fclose(ext_file);
    }
    return return_code;
}

RC_t FILES_CREATOR__create_ent_file(const char *file_path, symbol_table_t *symbol_table)
{
    RC_t return_code = UNINITIALIZED;
    FILE *ent_file = NULL;
    int total_entries = 0;
    int i = 0;

    if (file_path == NULL || symbol_table == NULL)
    {
        return_code = FILES_CREATOR__CREATE_ENT_FILE__NULL_ARGUMENT;
        goto Exit;
    }

    for (i = 0; i < MAX_SYMBOLS; i++)
    {
        if (symbol_table->symbols[i].symbol_name == NULL)
        {
            continue;
        }
        if (symbol_table->symbols[i].attributes[1] == SYMBOL_ENTRY)
        {
            total_entries++;
        }
    }

    if (total_entries == 0)
    {
        return_code = FILES_CREATOR__NO_ENTRIES_TO_WRITE;
        goto Exit;
    }

    ent_file = fopen(file_path, "w");
    if (ent_file == NULL)
    {
        return_code = FILES_CREATOR__CREATE_ENT_FILE__FOPEN_ERROR;
        goto Exit;
    }


    for (i = 0; i < MAX_SYMBOLS; i++)
    {
        if (symbol_table->symbols[i].symbol_name == NULL)
        {
            continue;
        }
        if (symbol_table->symbols[i].attributes[1] == SYMBOL_ENTRY)
        {
            fprintf(ent_file, "%s %04d\n", symbol_table->symbols[i].symbol_name, symbol_table->symbols[i].ic_value);
        }
    }

    return_code = SUCCESS;
Exit:
    if (ent_file != NULL)
    {
        fclose(ent_file);
    }
    return return_code;
}

RC_t files_creator__get_obj_file_header(parsed_lines_t *parsed_lines, int *instruction_image_size, int *data_image_size)
{
    RC_t return_code = UNINITIALIZED;
    int i = 0;

    if (parsed_lines == NULL || instruction_image_size == NULL || data_image_size == NULL)
    {
        return_code = FILES_CREATOR__GET_OBJ_FILE_HEADER__NULL_ARGUMENT;
        goto Exit;
    }

    for (i = 0; i < MAX_LINES_IN_FILE; i++)
    {
        if (parsed_lines->line[i].line_type == LINE_TYPE__UNINITIALIZED)
        {
            break;
        }
        if (parsed_lines->line[i].line_type == LINE_TYPE__INSTRUCTION)
        {
            (*instruction_image_size)++;
            if (parsed_lines->line[i].operands[0] != NULL)  
            {
                (*instruction_image_size)++;
            }
            if (parsed_lines->line[i].operands[1] != NULL)
            {
                (*instruction_image_size)++;
            }
        }
        if (parsed_lines->line[i].line_type == LINE_TYPE__STRING)
        {
            (*data_image_size) += (strlen(parsed_lines->line[i].string) + NULL_TERMINATOR_SIZE);
        }
        if (parsed_lines->line[i].line_type == LINE_TYPE__DATA)
        {
            (*data_image_size) += parsed_lines->line[i].numbers_count;
        }
    }

    return_code = SUCCESS;
Exit:
    return return_code;

}

RC_t FILES_CREATOR__create_asm_file(const char *file_path, encoded_lines_t *encoded_lines ,parsed_lines_t *parsed_lines)
{
    RC_t return_code = UNINITIALIZED;
    int instruction_image_size = 0;
    int data_image_size = 0;
    FILE *asm_file = NULL;
    int ic = 100;
    int i = 0;
    int j = 0;

    if (file_path == NULL || encoded_lines == NULL)
    {
        return_code = FILES_CREATOR__CREATE_ASM_FILE__NULL_ARGUMENT;
    }

    asm_file = fopen(file_path, "w");
    if (asm_file == NULL)
    {
        return_code = FILES_CREATOR__CREATE_ASM_FILE__FOPEN_ERROR;
        goto Exit;
    }

    EXIT_ON_ERROR(files_creator__get_obj_file_header(parsed_lines, &instruction_image_size, &data_image_size), &return_code);
    fprintf(asm_file, "%d %d\n", instruction_image_size, data_image_size);

    for (i = 0; i < MAX_LINES_IN_FILE; i++)
    {
        if (encoded_lines->encoded_line[i].words_count == 0)
        {
            break;
        }
        for (j = 0; j < encoded_lines->encoded_line[i].words_count; j++)
        {
            fprintf(asm_file, "%04d %03X %c\n", ic, encoded_lines->encoded_line[i].encoded_line[j] & 0xFFF, encoded_lines->encoded_line[i].words_type[j]);
            ic++;
        }
    }

    return_code = SUCCESS;
Exit:
    if (asm_file != NULL)
    {
        fclose(asm_file);
    }
    return return_code;
}