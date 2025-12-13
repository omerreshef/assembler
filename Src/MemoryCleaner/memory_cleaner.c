#include <stdlib.h>
#include <string.h>

#include "memory_cleaner__exports.h"

void MEMORY_CLEANER__clean_allocated_memory(parsed_lines_t *parsed_lines, encoded_lines_t *encoded_lines)
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
        for (l = 0; l < MAX_LINES_IN_FILE; l++)
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
            }
        }
    }

}