#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common__exports.h"
#include "line_parser__exports.h"
#include "file__exports.h"
#include "architecture__exports.h"
#include "first_pass__exports.h"
#include "first_pass__internals.h"

RC_t first_pass__detect_symbols_duplication(symbol_table_t *symbol_table, int symbols_amount)
{
    RC_t return_code = UNINITIALIZED;
    int i = 0;
    int j = 0;

    if (symbol_table == NULL)
    {
        return_code = FIRST_PASS__DETECT_SYMBOLS_DUPLICATION__NULL_ARGUMENT;
        goto Exit;
    }

    for (i = 0; i < symbols_amount; i++)
    {
        for (j = 0; j < symbols_amount; j++)
        {
            if (i != j && strcmp(symbol_table->symbols[i].symbol_name, symbol_table->symbols[j].symbol_name) == 0)
            {
                return_code = FIRST_PASS__DETECT_SYMBOLS_DUPLICATION__FOUND_DUPLICATION;
                goto Exit;
            }
        }
    }
    return_code = SUCCESS;
Exit:
    return return_code;
}

RC_t first_pass__detect_entries_duplication(program_entries_t *entries_list, int entries_amount)
{
    RC_t return_code = UNINITIALIZED;
    int i = 0;
    int j = 0;

    if (entries_list == NULL)
    {
        return_code = FIRST_PASS__DETECT_ENTRIES_DUPLICATION__NULL_ARGUMENT;
        goto Exit;
    }

    for (i = 0; i < entries_amount; i++)
    {
        for (j = 0; j < entries_amount; j++)
        {
            if (i != j && strcmp(entries_list->entries[i].entry_name, entries_list->entries[j].entry_name) == 0)
            {
                return_code = FIRST_PASS__DETECT_ENTRIES_DUPLICATION__FOUND_DUPLICATION;
                goto Exit;
            }
        }
    }
    return_code = SUCCESS;
Exit:
    return return_code;
}

RC_t first_pass__allocate_structures_memory(parsed_lines_t *parsed_lines, symbol_table_t *symbol_table, program_entries_t *entries_list, encoded_lines_t *encoded_lines, int lines_amount)
{
    RC_t return_code = UNINITIALIZED;
    int line_index = 0;
    int symbols_amount = 0;
    int entries_amount = 0;

    if (parsed_lines == NULL || symbol_table == NULL || entries_list == NULL || encoded_lines == NULL)
    {
        return_code = FIRST_PASS__ALLOCATE_STRUCTURES_MEMORY__NULL_ARGUMENT;
        goto Exit;
    }

    /* Count symbols and entries amount for the size of symbol_table and entries_list */
    for (line_index = 0; line_index < MAX_LINES_IN_FILE; line_index++)
    {
        switch (parsed_lines->line[line_index].line_type)
        {
            case LINE_TYPE__INSTRUCTION:
                if (parsed_lines->line[line_index].label != NULL)
                {
                    symbols_amount++;
                }
                break;
            case LINE_TYPE__STRING:
                if (parsed_lines->line[line_index].label != NULL)
                {
                    symbols_amount++;
                }
                break;
            case LINE_TYPE__DATA:
                if (parsed_lines->line[line_index].label != NULL)
                {
                    symbols_amount++;
                }
                break;
            case LINE_TYPE__EXTERN:
                symbols_amount++;
                break;
            case LINE_TYPE__ENTRY:
                entries_amount++;
                break;
            default:
                break;
        }
    }

    if (symbols_amount > 0)
    {
        symbol_table->symbols = (symbol_t *)malloc(sizeof(symbol_t) * symbols_amount);
        EXIT_IF_NULL(symbol_table->symbols, FIRST_PASS__ALLOCATE_STRUCTURES_MEMORY__MEMORY_ALLOCATION_FAILURE);
        /* zero-initialize so unused entries are NULL/0 */
        memset(symbol_table->symbols, 0, sizeof(symbol_t) * symbols_amount);
        symbol_table->symbols_amount = symbols_amount;
    }
    else
    {
        symbol_table->symbols = NULL;
        symbol_table->symbols_amount = 0;
    }

    if (entries_amount > 0)
    {
        entries_list->entries = (program_entry_t *)malloc(sizeof(program_entry_t) * entries_amount);
        EXIT_IF_NULL(entries_list->entries, FIRST_PASS__ALLOCATE_STRUCTURES_MEMORY__MEMORY_ALLOCATION_FAILURE);
        memset(entries_list->entries, 0, sizeof(program_entry_t) * entries_amount);
        entries_list->entries_amount = entries_amount;
    }
    else
    {
        entries_list->entries = NULL;
        entries_list->entries_amount = 0;
    }

    if (lines_amount > 0)
    {
        encoded_lines->encoded_line = (encoded_line_t *)malloc(sizeof(encoded_line_t) * lines_amount);
        EXIT_IF_NULL(encoded_lines->encoded_line, FIRST_PASS__ALLOCATE_STRUCTURES_MEMORY__MEMORY_ALLOCATION_FAILURE);
        memset(encoded_lines->encoded_line, 0, sizeof(encoded_line_t) * lines_amount);
        encoded_lines->encoded_lines_amount = lines_amount;
    }
    else
    {
        encoded_lines->encoded_line = NULL;
        encoded_lines->encoded_lines_amount = 0;
    }
    return_code = SUCCESS;
Exit:
    return return_code;
}

RC_t FIRST_PASS__process(char *input_file_path, int *instruction_counter, int *data_counter, parsed_lines_t *parsed_lines, symbol_table_t *symbol_table, program_entries_t *entries_list, encoded_lines_t *encoded_lines)
{
    RC_t return_code = UNINITIALIZED;
    FILE *input_file = NULL;
    bool is_line_valid = true;
    char line_buffer[MAX_LINE_LENGTH] = {0};
    int line_index = 0;
    int lines_amount = 0;
    int symbol_index = 0;
    int entry_index = 0;
    int opcode_size = 0;

    if (input_file_path == NULL || instruction_counter == NULL || data_counter == NULL || parsed_lines == NULL || symbol_table == NULL || entries_list == NULL)
    {
        return_code = FIRST_PASS__PROCESS__NULL_ARGUMENT;
        goto Exit;
    }

    /* Clear entries list before adding new entries */
    memset(entries_list, 0, sizeof(program_entries_t));

    /* Clear symbol table before adding new symbols */
    memset(symbol_table, 0, sizeof(symbol_table_t));

    *instruction_counter = INITIAL_IC_ADDRESS;
    *data_counter = INITIAL_DC_ADDRESS;

    EXIT_ON_ERROR(FILE__open(input_file_path, &input_file, "r"), &return_code);


    for (line_index = 0; line_index < MAX_LINES_IN_FILE; line_index++)
    {
        return_code = FILE__read_line(input_file, line_buffer, sizeof(line_buffer));
        if (return_code == FILE__READ_LINE__EOF_REACHED)
        {
            break; /* End of file reached */
        }
        else if (return_code != SUCCESS)
        {
            goto Exit;
        }
        return_code = LINE_PARSER__parse_line(line_buffer, &parsed_lines->line[line_index]);
        if (return_code != SUCCESS)
        {
            is_line_valid = false;
            goto Exit;
        }
    }

    if (!is_line_valid)
    {
        return_code = FIRST_PASS__PROCESS__INVALID_LINE_FOUND;
        goto Exit;
    }

    lines_amount = line_index;
    EXIT_ON_ERROR(first_pass__allocate_structures_memory(parsed_lines, symbol_table, entries_list, encoded_lines, lines_amount), &return_code);

    for (line_index = 0; line_index < MAX_LINES_IN_FILE; line_index++)
    {
        if (parsed_lines->line[line_index].line_type == LINE_TYPE__UNINITIALIZED)
        {
            break; /* No more parsed lines */
        }
        if (parsed_lines->line[line_index].line_type == LINE_TYPE__EMPTY)
        {
            continue; /* Skip empty lines */
        }

        switch (parsed_lines->line[line_index].line_type)
        {
            case LINE_TYPE__INSTRUCTION:
                if (parsed_lines->line[line_index].label != NULL)
                {
                    symbol_table->symbols[symbol_index].symbol_name = malloc(strlen(parsed_lines->line[line_index].label) + NULL_TERMINATOR_SIZE);
                    EXIT_IF_NULL(symbol_table->symbols[symbol_index].symbol_name, FIRST_PASS__PROCESS__MEMORY_ALLOCATION_FAILURE);
                    memset(symbol_table->symbols[symbol_index].symbol_name, 0, strlen(parsed_lines->line[line_index].label) + NULL_TERMINATOR_SIZE);
                    memcpy(symbol_table->symbols[symbol_index].symbol_name, parsed_lines->line[line_index].label, strlen(parsed_lines->line[line_index].label));
                    symbol_table->symbols[symbol_index].ic_value = *instruction_counter;
                    symbol_table->symbols[symbol_index].attributes[0] = SYMBOL_CODE;
                    symbol_index++;
                }
                EXIT_ON_ERROR(ARCHITECTURE__get_opcode_size(parsed_lines->line[line_index].opcode, &opcode_size), &return_code);
                parsed_lines->line[line_index].line_ic = *instruction_counter;
                (*instruction_counter) += opcode_size;
                break;
            case LINE_TYPE__EXTERN:
                if (parsed_lines->line[line_index].label != NULL)
                {
                    return_code = FIRST_PASS__PROCESS__FOUND_EXTERN_WITH_LABEL;
                    goto Exit;
                }
                symbol_table->symbols[symbol_index].symbol_name = malloc(strlen(parsed_lines->line[line_index].extern_name) + NULL_TERMINATOR_SIZE);
                EXIT_IF_NULL(symbol_table->symbols[symbol_index].symbol_name, FIRST_PASS__PROCESS__MEMORY_ALLOCATION_FAILURE);
                memset(symbol_table->symbols[symbol_index].symbol_name, 0, strlen(parsed_lines->line[line_index].extern_name) + NULL_TERMINATOR_SIZE);
                memcpy(symbol_table->symbols[symbol_index].symbol_name, parsed_lines->line[line_index].extern_name, strlen(parsed_lines->line[line_index].extern_name));
                symbol_table->symbols[symbol_index].ic_value = 0;
                symbol_table->symbols[symbol_index].attributes[0] = SYMBOL_EXTERNAL;
                symbol_index++;
                break;
            case LINE_TYPE__ENTRY:
                entries_list->entries[entry_index].entry_name = malloc(strlen(parsed_lines->line[line_index].entry_name) + NULL_TERMINATOR_SIZE);
                EXIT_IF_NULL(entries_list->entries[entry_index].entry_name, FIRST_PASS__PROCESS__MEMORY_ALLOCATION_FAILURE);
                memset(entries_list->entries[entry_index].entry_name, 0, strlen(parsed_lines->line[line_index].entry_name) + NULL_TERMINATOR_SIZE);
                memcpy(entries_list->entries[entry_index].entry_name, parsed_lines->line[line_index].entry_name, strlen(parsed_lines->line[line_index].entry_name) + NULL_TERMINATOR_SIZE);
                entry_index++;
                break;
            case LINE_TYPE__STRING:
                if (parsed_lines->line[line_index].string[0] == '\0')
                {
                    return FIRST_PASS__PROCESS__INVALID_STRING;
                }
                if (parsed_lines->line[line_index].label != NULL)
                {
                    symbol_table->symbols[symbol_index].symbol_name = malloc(strlen(parsed_lines->line[line_index].label) + NULL_TERMINATOR_SIZE);
                    EXIT_IF_NULL(symbol_table->symbols[symbol_index].symbol_name, FIRST_PASS__PROCESS__MEMORY_ALLOCATION_FAILURE);
                    memset(symbol_table->symbols[symbol_index].symbol_name, 0, strlen(parsed_lines->line[line_index].label) + NULL_TERMINATOR_SIZE);
                    memcpy(symbol_table->symbols[symbol_index].symbol_name, parsed_lines->line[line_index].label, strlen(parsed_lines->line[line_index].label));
                    symbol_table->symbols[symbol_index].attributes[0] = SYMBOL_DATA;
                    symbol_table->symbols[symbol_index].ic_value = *data_counter;
                    symbol_index++;
                }
                (*data_counter) += (int)strlen(parsed_lines->line[line_index].string) + NULL_TERMINATOR_SIZE;
                break;
            case LINE_TYPE__DATA:
                if (parsed_lines->line[line_index].label != NULL)
                {
                    symbol_table->symbols[symbol_index].symbol_name = malloc(strlen(parsed_lines->line[line_index].label) + NULL_TERMINATOR_SIZE);
                    EXIT_IF_NULL(symbol_table->symbols[symbol_index].symbol_name, FIRST_PASS__PROCESS__MEMORY_ALLOCATION_FAILURE);
                    memset(symbol_table->symbols[symbol_index].symbol_name, 0, strlen(parsed_lines->line[line_index].label) + NULL_TERMINATOR_SIZE);
                    memcpy(symbol_table->symbols[symbol_index].symbol_name, parsed_lines->line[line_index].label, strlen(parsed_lines->line[line_index].label));
                    symbol_table->symbols[symbol_index].attributes[0] = SYMBOL_DATA;
                    symbol_table->symbols[symbol_index].ic_value = *data_counter;
                    symbol_index++;
                }
                (*data_counter) += parsed_lines->line[line_index].numbers_count;
                break;
            case LINE_TYPE__EMPTY:
                /* No action needed for these line types in the first pass */
                break;
            default:
                return_code = FIRST_PASS__PROCESS__UNKNOWN_LINE_TYPE;
                goto Exit;
        }
    }

    EXIT_ON_ERROR(first_pass__detect_symbols_duplication(symbol_table, symbol_index), &return_code);
    EXIT_ON_ERROR(first_pass__detect_entries_duplication(entries_list, entry_index), &return_code);

    for (line_index = 0; line_index < symbol_index; line_index++)
    {
        if (symbol_table->symbols[line_index].attributes[0] == SYMBOL_DATA)
        {
            symbol_table->symbols[line_index].ic_value += *instruction_counter;
        }
    }


    return_code = SUCCESS;
Exit:
    return return_code;
}