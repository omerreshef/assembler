#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "assembler__exports.h"
#include "line_parser__exports.h"
#include "common__exports.h"
#include "architecture__exports.h"
#include "second_pass__exports.h"
#include "second_pass__internals.h"

RC_t second_pass__finalize_symbol_table(symbol_table_t *symbol_table, program_entries_t *entries_list)
{
    RC_t return_code = UNINITIALIZED;
    int symbol_index = 0;
    int entry_index = 0;
    bool found_symbol = false;

    for (entry_index = 0; entry_index < MAX_ENTRIES; entry_index++)
    {
        found_symbol = false;

        if (entries_list->entries[entry_index].entry_name == NULL)
        {
            break;
        }

        for (symbol_index = 0; symbol_index < MAX_SYMBOLS; symbol_index++)
        {

            if (symbol_table->symbols[symbol_index].symbol_name == NULL)
            {
                continue;   
            }

            if (strcmp(symbol_table->symbols[symbol_index].symbol_name, entries_list->entries[entry_index].entry_name) == 0)
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

        if (!found_symbol)
        {
            return_code = SECOND_PASS__FINALIZE_SYMBOL_TABLE__SYMBOL_NOT_FOUND;
            goto Exit;
        }
    }
    
    return_code = SUCCESS;
Exit:
    return return_code;

}

RC_t second_pass__enter_new_extern_usage(extern_usages_t *extern_usages, int operand_ic, char *symbol_name)
{
    RC_t return_code = UNINITIALIZED;
    int i = 0;

    if (extern_usages == NULL || symbol_name == NULL)
    {
        return_code = SECOND_PASS_ENTER_NEW_EXTERN_USAGE__NULL_ARGUMENT;
        goto Exit;
    }

    for (i = 0; i < MAX_EXTERN_USAGES; i++)
    {
        if (extern_usages->extern_usage[i].name == NULL)
        {
            break;
        }
    }

    if (i >= MAX_EXTERN_USAGES)
    {
        return_code = SECOND_PASS_ENTER_NEW_EXTERN_USAGE__REACHED_MAX_EXTERN_USAGES;
        goto Exit;
    }

    extern_usages->extern_usage[i].ic = operand_ic;
    extern_usages->extern_usage[i].name = symbol_name;

    return_code = SUCCESS;
Exit:
    return return_code;
}


RC_t second_pass__resolve_operand(char *operand, symbol_table_t *symbol_table, int operand_ic, int *operand_value, operand_type_t *operand_type, extern_usages_t *extern_usages)
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
            EXIT_ON_ERROR(second_pass__enter_new_extern_usage(extern_usages, operand_ic, symbol_table->symbols[i].symbol_name), &return_code);
            *operand_type = OPERAND_EXTERNAL;
        }
        else
        {
            *operand_value = symbol_table->symbols[i].ic_value;
            *operand_type = OPERAND_DIRECT;
        }
    }

    return_code = SUCCESS;

Exit:
    return return_code;
}

RC_t second_pass__handle_instruction_operand(int operand_value, operand_type_t operand_type, int *encoded_operand)
{
    RC_t return_code = UNINITIALIZED;

    if (encoded_operand == NULL)
    {
        return_code = SECOND_PASS__HANDLE_INSTRUCTION_OPERAND__NULL_ARGUMENT;
        goto Exit;
    }

    switch (operand_type)
    {
    case OPERAND_REGISTER:
    case OPERAND_IMMEDIATE:
    case OPERAND_DIRECT:
    case OPERAND_RELATIVE:
        *encoded_operand = operand_value;
        break;
    default:
        return_code = SECOND_PASS__HANDLE_INSTRUCTION_OPERAND__UNSUPPORTED_OPERAND_TYPE;
        break;
    }

    return_code = SUCCESS;
Exit:
    return return_code;
}

RC_t second_pass__encode_instruction(parsed_line_t *instruction,
                                     symbol_table_t *symbol_table,
                                     int first_operand_value,
                                     int second_operand_value,
                                     operand_type_t first_operand_type,
                                     operand_type_t second_operand_type,
                                     int encoded_instruction[MAX_WORDS_IN_INSTRUCTION],
                                     int *words_count)
{
    RC_t return_code =  UNINITIALIZED;
    opcode_t opcode_details = {0};
    int opcode_value = 0;
    int opcode_funct = -1;
    int source_address_mode = 0;
    int destination_address_mode = 0;
    int operands_number = 0;
    int first_operand_encode = 0;
    int second_operand_encode = 0;

    EXIT_ON_ERROR(ARCHITECTURE__get_opcode_details(instruction->opcode, &opcode_details), &return_code);
    opcode_value = opcode_details.opcode;
    opcode_funct = opcode_details.funct;
    operands_number = opcode_details.opcode_operands_number;

    switch (operands_number)
    {
    case 0:
        encoded_instruction[0] = MAKE_MACHINE_CODE(opcode_value, opcode_funct, 0, 0);
        *words_count = 1;
        break;

    case 1:
        encoded_instruction[0] = MAKE_MACHINE_CODE(opcode_value, opcode_funct, first_operand_type, 0);
        EXIT_ON_ERROR(second_pass__handle_instruction_operand(first_operand_value, first_operand_type, &encoded_instruction[1]), &return_code);
        *words_count = 2;
        break;
    
    case 2:
        encoded_instruction[0] = MAKE_MACHINE_CODE(opcode_value, opcode_funct, first_operand_type, second_operand_type);
        EXIT_ON_ERROR(second_pass__handle_instruction_operand(first_operand_value, first_operand_type, &encoded_instruction[1]), &return_code);
        EXIT_ON_ERROR(second_pass__handle_instruction_operand(second_operand_value, second_operand_type, &encoded_instruction[2]), &return_code);
        *words_count = 3;
        break;
    
    default:
        return_code = SECOND_PASS__ENCODE_INSTRUCTION__INVALID_OPERANDS_NUMBER;
        goto Exit;
        break;
    }

    return_code = SUCCESS;
Exit:
    return return_code;
}

RC_t second_pass__encode_line(extern_usages_t *extern_usages, parsed_line_t *parsed_line, symbol_table_t *symbol_table, encoded_line_t *encoded_line)
{
    RC_t return_code =  UNINITIALIZED;
    int i = 0;
    int first_operand_value = 0;
    int second_operand_value = 0;
    operand_type_t first_operand_type = OPERAND_UNINITIALIZED;
    operand_type_t second_operand_type = OPERAND_UNINITIALIZED;

    if (parsed_line == NULL)
    {
        return_code = SECOND_PASS__ENCODE_LINE__NULL_ARGUMENT;
        goto Exit;
    }

    switch (parsed_line->line_type)
    {
        case LINE_TYPE__INSTRUCTION:
            encoded_line->encoded_line = malloc(sizeof(int) * MAX_WORDS_IN_INSTRUCTION);
            EXIT_IF_NULL(encoded_line->encoded_line, SECOND_PASS__ENCODE_INSTRUCTION__FAILED_MALLOC);
            if (parsed_line->operands[0] != NULL)
            {
                EXIT_ON_ERROR(second_pass__resolve_operand(parsed_line->operands[0], symbol_table, parsed_line->line_ic + 1, &first_operand_value, &first_operand_type, extern_usages), &return_code);
            }

            if (parsed_line->operands[1] != NULL)
            {
                EXIT_ON_ERROR(second_pass__resolve_operand(parsed_line->operands[1], symbol_table, parsed_line->line_ic + 2, &second_operand_value, &second_operand_type, extern_usages), &return_code);
            }
            EXIT_ON_ERROR(second_pass__encode_instruction(parsed_line, symbol_table, first_operand_value, second_operand_value, first_operand_type, second_operand_type, encoded_line->encoded_line, &encoded_line->words_count) , &return_code);
            break;
        
        case LINE_TYPE__DATA:
            encoded_line->encoded_line = malloc(sizeof(int) * parsed_line->numbers_count);
            EXIT_IF_NULL(encoded_line->encoded_line, SECOND_PASS__ENCODE_INSTRUCTION__FAILED_MALLOC);
            for (i = 0; i < parsed_line->numbers_count; i++)
            {
                (encoded_line->encoded_line)[i] = parsed_line->numbers[i];
            }
            encoded_line->words_count = parsed_line->numbers_count;
            break;

        case LINE_TYPE__STRING:
            encoded_line->encoded_line = malloc(sizeof(int) * (strlen(parsed_line->string) + NULL_TERMINATOR_SIZE));
            EXIT_IF_NULL(encoded_line->encoded_line, SECOND_PASS__ENCODE_INSTRUCTION__FAILED_MALLOC);
            for (i = 0; i < strlen(parsed_line->string); i++)
            {
                (encoded_line->encoded_line)[i] = (int)parsed_line->string[i];
            }
            (encoded_line->encoded_line)[i] = 0; /* Null terminator */
            encoded_line->words_count = strlen(parsed_line->string) + NULL_TERMINATOR_SIZE;
            break;
        
        case LINE_TYPE__EMPTY:
        case LINE_TYPE__EXTERN:
        case LINE_TYPE__ENTRY:
            return_code = SECOND_PASS__ENCODE_LINE__UNSOPPORTED_LINE_TYPE;
            goto Exit;
            break;
        
        default:
            return_code = SECOND_PASS__ENCODE_LINE__INVALID_LINE_TYPE;
            goto Exit;
            break;
    }


    return_code = SUCCESS;
Exit:
    return return_code;
}

RC_t SECOND_PASS__process(program_encoded_lines_t *encoded_lines, extern_usages_t *extern_usages, parsed_lines_t *parsed_lines, symbol_table_t *symbol_table, program_entries_t *entries_list)
{
    RC_t return_code = UNINITIALIZED;
    int i = 0;

    if (parsed_lines == NULL || symbol_table == NULL || entries_list == NULL)
    {
        return_code = SECOND_PASS__PROCESS__NULL_ARGUMENT;
    }

    EXIT_ON_ERROR(second_pass__finalize_symbol_table(symbol_table, entries_list), &return_code);
    for (i = 0; i < MAX_LINES_IN_FILE; i++)
    {
        EXIT_ON_ERROR(second_pass__encode_line(extern_usages, &parsed_lines->line[i], symbol_table, &encoded_lines->encoded_line[i]), &return_code);
    }

Exit:
    return return_code;
}