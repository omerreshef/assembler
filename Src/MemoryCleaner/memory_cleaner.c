#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "memory_cleaner__exports.h"

void MEMORY_CLEANER__clean_allocated_memory(parsed_lines_t *parsed_lines, encoded_lines_t *encoded_lines, extern_usages_t *extern_usages, symbol_table_t *symbol_table, program_entries_t *entries_list)
{
    int i = 0;
    int l = 0;


    if (parsed_lines != NULL)
    {
        for (i = 0; i < MAX_LINES_IN_FILE; i++)
        {
            if (parsed_lines->line[i].line_type == LINE_TYPE__UNINITIALIZED)
            {
                continue;
            }
            if (parsed_lines->line[i].line_type == LINE_TYPE__EMPTY)
            {
                continue;
            }
            if (parsed_lines->line[i].operands[0] != NULL)
            {
                free(parsed_lines->line[i].operands[0]);
                parsed_lines->line[i].operands[0] = NULL;
            }
            if (parsed_lines->line[i].operands[1] != NULL)
            {
                free(parsed_lines->line[i].operands[1]);
                parsed_lines->line[i].operands[1] = NULL;
            }
            if (parsed_lines->line[i].label != NULL)
            {
                free(parsed_lines->line[i].label);
                parsed_lines->line[i].label = NULL;
            }
            if (parsed_lines->line[i].string != NULL)
            {
                free(parsed_lines->line[i].string);
                parsed_lines->line[i].string = NULL;
            }
            if (parsed_lines->line[i].opcode != NULL)
            {
                free(parsed_lines->line[i].opcode);
                parsed_lines->line[i].opcode = NULL;
            }
            if (parsed_lines->line[i].entry_name != NULL)
            {
                free(parsed_lines->line[i].entry_name);
                parsed_lines->line[i].entry_name = NULL;
            }
            if (parsed_lines->line[i].extern_name != NULL)
            {
                free(parsed_lines->line[i].extern_name);
                parsed_lines->line[i].extern_name = NULL;
            }
            if (parsed_lines->line[i].numbers != NULL)
            {
                free(parsed_lines->line[i].numbers);
                parsed_lines->line[i].numbers = NULL;
            }
        }
    }

    if (encoded_lines != NULL)
    {
        if (encoded_lines->encoded_line != NULL)
        {
            for (l = 0; l < encoded_lines->encoded_lines_amount; l++)
            {
                if (encoded_lines->encoded_line[l].encoded_line != NULL)
                {
                    free(encoded_lines->encoded_line[l].encoded_line);
                    encoded_lines->encoded_line[l].encoded_line = NULL;
                }
                if (encoded_lines->encoded_line[l].words_type != NULL)
                {
                    free(encoded_lines->encoded_line[l].words_type);
                    encoded_lines->encoded_line[l].words_type = NULL;
                    encoded_lines->encoded_line[l].words_count = 0;
                }
            }
            free(encoded_lines->encoded_line);
            encoded_lines->encoded_line = NULL;
            encoded_lines->encoded_lines_amount = 0;
        }

    }

    if (symbol_table != NULL)
    {
        if (symbol_table->symbols != NULL)
        {
            free(symbol_table->symbols);
            symbol_table->symbols = NULL;
            symbol_table->symbols_amount = 0;
        }
    }


    if (entries_list != NULL)
    {
        if (entries_list->entries != NULL)
        {
            free(entries_list->entries);
            entries_list->entries = NULL;
            entries_list->entries_amount = 0;
        }
    }
    
    if (extern_usages != NULL)
    {
        if (extern_usages->extern_usage != NULL)
        {
            for (i = 0; i < extern_usages->extern_usages_amount; i++)
            {
                if (extern_usages->extern_usage[i].name != NULL)
                {
                    free(extern_usages->extern_usage[i].name);
                    extern_usages->extern_usage[i].name = NULL;
                }
                extern_usages->extern_usage[i].ic = 0;
            }
            free(extern_usages->extern_usage);
            extern_usages->extern_usage = NULL;
            extern_usages->extern_usages_amount = 0;
        }
    }

}