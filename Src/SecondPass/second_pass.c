#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "assembler__exports.h"
#include "line_parser__exports.h"
#include "common__exports.h"
#include "architecture__exports.h"

RC_t second_pass__finalize_symbol_table(symbol_table_t *symbol_table, char *entries_list[MAX_ENTRIES])
{
    RC_t return_code = UNINITIALIZED;
    int symbol_index = 0;
    int entry_index = 0;
    bool found_symbol = false;

    for (entry_index = 0; entry_index < MAX_ENTRIES; entry_index++)
    {
        found_symbol = false;

        if (entries_list[entry_index == NULL])
        {
            break;
        }

        for (symbol_index = 0; symbol_index < MAX_SYMBOLS; symbol_index++)
        {

            if (symbol_table->symbols[symbol_index].symbol_name == NULL)
            {
                continue;   
            }

            if (strcmp(symbol_table->symbols[symbol_index].symbol_name, entries_list[entry_index]) == 0)
            {
                if (symbol_table->symbols[symbol_index].attributes[0] == SYMBOL_EXTERNAL)
                {
                    return SECOND_PASS__FINALIZE_SYMBOL_TABLE__ENTRY_ON_EXTERN;
                }
                symbol_table->symbols[symbol_index].attributes[1] = SYMBOL_ENTRY;
                found_symbol = true;
                break;
            }
        }
    }
    if (!found_symbol)
    {
        return_code = SECOND_PASS__FINALIZE_SYMBOL_TABLE__SYMBOL_NOT_FOUND;
        goto Exit;
    }
    
    return_code = SUCCESS;
Exit:
    return return_code;

}

RC_t second_pass__resolve_operand(char *operand, symbol_table_t *symbol_table, int operand_ic, int *operand_value, operand_type_t *operand_type)
{
    RC_t return_code = UNINITIALIZED;
    char *end_pointer = NULL;
    bool is_register = false;
    bool is_label = false;
    int label_ic = 0;
    int i = 0;

    if (operand == NULL || symbol_table == NULL || operand_value == NULL || operand_type == NULL)
    {
        return_code = SECOND_PASS__RESOLVE_OPERAND__NULL_ARGUMENT;
        goto Exit;
    }

    EXIT_ON_ERROR(ARCHITECTURE__is_register(operand, &is_register), &return_code);

    if (is_register)
    {
        EXIT_ON_ERROR(ARCHITECTURE__get_register_value(operand, operand_value), &return_code);
        *operand_type = OPERAND_REGISTER;
    }

    else if (operand[0] == '#')
    {
        operand++; /* Skip '#' character */
        *operand_value = (int)strtol(operand, &end_pointer, 10);
        if (*end_pointer != '\0')
        {
            return_code = SECOND_PASS__RESOLVE_OPERAND__STRTOL_ERROR;
            goto Exit;
        }
        *operand_type = OPERAND_IMMEDIATE;
    }

    else if (operand[0] == '%')
    {
        operand++; /* Skip '%' character */
        for (i = 0; i < MAX_SYMBOLS; i++)
        {
            if (symbol_table->symbols[i].symbol_name == NULL)
            {
                continue;
            }
            if (strcmp(operand, symbol_table->symbols[i].symbol_name) == 0)
            {
                is_label = true;
                break;
            }
        }
        if (!is_label)
        {
            return_code = SECOND_PASS__RESOLVE_OPERAND__LABEL_NOT_FOUND;
            goto Exit;
        }
        *operand_type = OPERAND_RELATIVE;
        *operand_value = symbol_table->symbols[i].ic_value - (operand_ic + 1);
    }

    else
    {
        for (i = 0; i < MAX_SYMBOLS; i++)
        {
            if (symbol_table->symbols[i].symbol_name == NULL)
            {
                continue;
            }
            if (strcmp(operand, symbol_table->symbols[i].symbol_name) == 0)
            {
                is_label = true;
                break;
            }
        }
        if (!is_label)
        {
            return_code = SECOND_PASS__RESOLVE_OPERAND__UNRECOGNIZED_OPERAND;
            goto Exit;
        }
        if (symbol_table->symbols[i].attributes[0] == SYMBOL_EXTERNAL)
        {
            *operand_value = 0;
            *operand_type = OPERAND_EXTERNAL;
        }
        else
        {
            *operand_value = symbol_table->symbols[i].ic_value;
            *operand_type = OPERAND_LABEL;
        }
    }

    return_code = SUCCESS;

Exit:
    return return_code;
}

RC_t second_pass__encode_instruction(char *opcode, int first_operand_value, int second_operand_value, operand_type_t first_operand_type, operand_type_t second_operand_type)
{
    RC_t return_code =  UNINITIALIZED;

    return_code = SUCCESS;
Exit:
    return return_code;
}

RC_t second_pass__encode_line(parsed_line_t *parsed_line, symbol_table_t *symbol_table)
{
    RC_t return_code =  UNINITIALIZED;
    int first_operand_value = 0;
    int second_operand_value = 0;
    operand_type_t first_operand_type = OPERAND_UNINITIALIZED;
    operand_type_t second_operand_type = OPERAND_UNINITIALIZED;

    if (parsed_line == NULL)
    {
        return_code = SECOND_PASS__ENCODE_INSTRUCTION__NULL_ARGUMENT;
        goto Exit;
    }

    switch (parsed_line->line_type)
    {
        case LINE_TYPE__INSTRUCTION:
            if (parsed_line->operands[0] != NULL)
            {
                EXIT_ON_ERROR(second_pass__resolve_operand(parsed_line->operands[0], symbol_table, parsed_line->line_ic, &first_operand_value, &first_operand_type), &return_code);
            }
        
            if (parsed_line->operands[1] != NULL)
            {
                EXIT_ON_ERROR(second_pass__resolve_operand(parsed_line->operands[1], symbol_table, parsed_line->line_ic, &second_operand_value, &second_operand_type), &return_code);
            }
            EXIT_ON_ERROR(second_pass__encode_instruction(parsed_line->opcode, first_operand_type, second_operand_type, first_operand_value, second_operand_value) , &return_code);
            break;
        
        case LINE_TYPE__DATA:
            break;
        
        default:
            break;
    }


    return_code = SUCCESS;
Exit:
    return return_code;
}

RC_t SECOND_PASS__process(parsed_line_t *parsed_lines, symbol_table_t *symbol_table, char *entries_list[MAX_ENTRIES])
{

}