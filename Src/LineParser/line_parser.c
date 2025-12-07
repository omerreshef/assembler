#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "common__exports.h"
#include "line_parser__internals.h"
#include "line_parser__exports.h"
#include "architecture__exports.h"

RC_t line_parser__parse_instruction_operands(const char* opcode, const char* arguments, parsed_line_t *parsed_line)
{
    RC_t return_code = UNINITIALIZED;
    int first_operand_length = 0;
    int second_operand_length = 0;
    char *comma_position = NULL;
    char *second_operand_start = NULL;

    if (opcode == NULL || arguments == NULL || parsed_line == NULL)
    {
        return_code = LINE_PARSER__PARSE_INSTRUCTION_OPERANDS__NULL_ARGUMENT;
        goto Exit;
    }

    if (strcmp(opcode, "mov") == 0 ||
        strcmp(opcode, "add") == 0 ||
        strcmp(opcode, "sub") == 0 ||
        strcmp(opcode, "lea") == 0 ||
        strcmp(opcode, "cmp") == 0)
    {
        comma_position = strchr(arguments, ',');
        /* Two operands expected */
        if (comma_position == NULL)
        {
            return_code = LINE_PARSER__PARSE_INSTRUCTION_OPERANDS__NO_COMMA_BETWEEN_OPERANDS;
            goto Exit;
        }

        first_operand_length = (int)(comma_position - arguments);
        parsed_line->operands[0] = malloc(first_operand_length + 1);
        EXIT_IF_NULL(parsed_line->operands[0], LINE_PARSER__PARSE_INSTRUCTION_OPERANDS__ALLOCATION_ERROR);
        (void)memset(parsed_line->operands[0], '\0', first_operand_length + 1);
        (void)memcpy(parsed_line->operands[0], arguments, first_operand_length);

        second_operand_start = comma_position + 1;
        second_operand_length = (int)strlen(second_operand_start);
        parsed_line->operands[1] = malloc(second_operand_length + 1);
        EXIT_IF_NULL(parsed_line->operands[1], LINE_PARSER__PARSE_INSTRUCTION_OPERANDS__ALLOCATION_ERROR);
        (void)memset(parsed_line->operands[1], '\0', second_operand_length + 1);
        (void)memcpy(parsed_line->operands[1], second_operand_start, second_operand_length);
    }
    else if (
        strcmp(opcode, "clr") == 0 ||
        strcmp(opcode, "not") == 0 ||
        strcmp(opcode, "inc") == 0 ||
        strcmp(opcode, "dec") == 0 ||
        strcmp(opcode, "jmp") == 0 ||
        strcmp(opcode, "bne") == 0 ||
        strcmp(opcode, "jsr") == 0 ||
        strcmp(opcode, "red") == 0 ||
        strcmp(opcode, "prn") == 0
    )
    {
        /* One operand expected */
        int operand_length = (int)strlen(arguments);
        parsed_line->operands[0] = malloc(operand_length + 1);
        EXIT_IF_NULL(parsed_line->operands[0], LINE_PARSER__PARSE_INSTRUCTION_OPERANDS__ALLOCATION_ERROR);
        (void)memset(parsed_line->operands[0], '\0', operand_length + 1);
        (void)memcpy(parsed_line->operands[0], arguments, operand_length);
    } else if (
        strcmp(opcode, "rts") == 0 ||
        strcmp(opcode, "stop") == 0
    )
    {
        if (strlen(arguments) != 0)
        {
            /* No operands expected */
            return_code = LINE_PARSER__PARSE_INSTRUCTION_OPERANDS__NO_OPERANDS_EXPECTED;
            goto Exit;
        }
    }

    return_code = SUCCESS;
Exit:
    if (return_code != SUCCESS)
    {
        if (parsed_line->operands[0] != NULL)
        {
            free(parsed_line->operands[0]);
        }
        if (parsed_line->operands[1] != NULL)
        {
            free(parsed_line->operands[1]);
        }
    }
    return return_code;
}

RC_t line_parser__parse_data_numbers(char* data, parsed_line_t *parsed_line)
{
    RC_t return_code = UNINITIALIZED;
    char *data_pointer = data;
    int numbers_amount = 0;
    int token_index = 0;
    int *numbers = NULL;
    char *token = NULL;
    char *end_pointer = NULL;
    char *numbers_copy = NULL;

    if (data == NULL || parsed_line == NULL)
    {
        return_code = LINE_PARSER__PARSE_DATA_NUMBERS__NULL_ARGUMENT;
        goto Exit;
    }

    /* Count commas to find number count */
    while (*data_pointer != '\0')
    {
        if (*data_pointer == ',')
        {
            numbers_amount++;
        }
        data_pointer++;
    }

    numbers_amount += 1; /* Account for the last number */

    if (numbers_amount == 0) 
    {
        return_code = LINE_PARSER__PARSE_DATA_NUMBERS__NO_DATA_VALUES;
        goto Exit;
    }

    numbers = malloc((numbers_amount) * sizeof(int));
    EXIT_IF_NULL(numbers, LINE_PARSER__PARSE_DATA_NUMBERS__ALLOCATION_ERROR);

    numbers_copy = malloc(strlen(data) + NULL_TERMINATOR_SIZE);
    EXIT_IF_NULL(numbers_copy, LINE_PARSER__PARSE_DATA_NUMBERS__ALLOCATION_ERROR);
    memset(numbers_copy, '\0', strlen(data) + NULL_TERMINATOR_SIZE);
    /* Dont copy the quotation character in the end of the string. Just the numbers */
    memcpy(numbers_copy, data, strlen(data) - QUOTATION_CHARACTER_LEN);


    token = strtok(numbers_copy, ",");
    while (token != NULL && token_index < numbers_amount) {
        numbers[token_index] = (int)strtol(token, &end_pointer, 10);
        if (end_pointer == token || *end_pointer != '\0')
        {
            /* The token is not a valid integer */
            return_code = LINE_PARSER__PARSE_DATA_NUMBERS__INVALID_DATA_VALUE;
            goto Exit;
        }
        token_index += 1;
        token = strtok(NULL, ",");
    }

    parsed_line->numbers = numbers;
    parsed_line->numbers_count = numbers_amount;

    return_code = SUCCESS;
Exit:
    if (numbers_copy != NULL)
    {
        free(numbers_copy);
    }
    if (return_code != SUCCESS && numbers != NULL)
    {
        free(numbers);
    }
    return return_code;
}

RC_t LINE_PARSER__parse_line(const char *line_buffer, parsed_line_t *parsed_line)
{
    RC_t return_code = UNINITIALIZED;
    int label_length = 0;
    int string_length = 0;
    int opcode_length = 0;
    char *label_end = NULL;
    char *string_start = NULL;
    char *string_end = NULL;
    char *opcode_end = NULL;
    char *line_pointer = (char *)line_buffer;
    char *extern_name = NULL;
    char *entry_name = NULL;
    bool is_opcode = false;

    if (line_buffer == NULL || parsed_line == NULL)
    {
        return_code = LINE_PARSER__PARSE_LINE__NULL_ARGUMENT;
        goto Exit;
    }

    /* If line is empty or a comment, set line type to EMPTY */
    if (line_pointer[0] == '\0' || line_pointer[0] == ';')
    {
        parsed_line->line_type = LINE_TYPE__EMPTY;
        return_code = SUCCESS;
        goto Exit;
    }

    /* Detect a label */
    label_end = strchr(line_pointer, ':');
    if (label_end != NULL)
    {
        label_length = (int)(label_end - line_pointer);
        parsed_line->label = malloc(label_length + 1);
        EXIT_IF_NULL(parsed_line->label, LINE_PARSER__PARSE_LINE__ALLOCATION_ERROR);
        (void)memset(parsed_line->label, '\0', label_length + 1);
        (void)memcpy(parsed_line->label, line_pointer, label_length);
        line_pointer += label_length + LABEL_SKIP_LENGTH;
    }

    /* Detect line type and handle the line content. */
    if (strstr(line_pointer, ".data"))
    {
        parsed_line->line_type = LINE_TYPE__DATA;
        line_pointer += strlen(".data") + SPACE_CHARACTER_LEN + QUOTATION_CHARACTER_LEN;
        
        EXIT_ON_ERROR(line_parser__parse_data_numbers(line_pointer, parsed_line), &return_code);
    } 
    else if (strstr(line_pointer, ".string"))
    {
        parsed_line->line_type = LINE_TYPE__STRING;
        string_start = strchr(line_pointer, '\"');
        if (string_start == NULL)
        {
            /* No opening quote found */
            return_code = LINE_PARSER__PARSE_LINE__NULL_ARGUMENT;
            goto Exit;
        }
        line_pointer = string_start + 1; /* Move past the opening quote */
        string_end = strchr(line_pointer, '\"');
        string_length = (int)(string_end - line_pointer);
        parsed_line->string = malloc(string_length + 1);
        memset(parsed_line->string, '\0', string_length + 1);
        EXIT_IF_NULL(parsed_line->string, LINE_PARSER__PARSE_LINE__ALLOCATION_ERROR);
        (void)memset(parsed_line->string, '\0', string_length + 1);
        (void)memcpy(parsed_line->string, line_pointer, string_length);

    } 
    else if (strstr(line_pointer, ".entry"))
    {
        if (label_end != NULL) 
        {
            return_code = LINE_PARSER__PARSE_LINE__LABEL_IN_ENTRY;
            goto Exit;
        }
        parsed_line->line_type = LINE_TYPE__ENTRY;
        entry_name = strchr(line_pointer, ' ');
        entry_name++; /* Skip space character to the start if the entry name*/
        parsed_line->entry_name = malloc(strlen(entry_name) + NULL_TERMINATOR_SIZE);
        memset(parsed_line->entry_name, '\0', strlen(entry_name) + NULL_TERMINATOR_SIZE);
        EXIT_IF_NULL(parsed_line->entry_name, LINE_PARSER__PARSE_LINE__ALLOCATION_ERROR);
        (void)memcpy(parsed_line->entry_name, entry_name, strlen(entry_name));
    }
    else if (strstr(line_pointer, ".extern"))
    {
        if (label_end != NULL) 
        {
            return_code = LINE_PARSER__PARSE_LINE__LABEL_IN_EXTERN;
            goto Exit;
        }
        parsed_line->line_type = LINE_TYPE__EXTERN;
        extern_name = strchr(line_pointer, ' ');
        extern_name++; /* Skip space character to the start if the extern name*/
        parsed_line->extern_name = malloc(strlen(extern_name) + NULL_TERMINATOR_SIZE);
        memset(parsed_line->extern_name, '\0', strlen(extern_name) + NULL_TERMINATOR_SIZE);
        EXIT_IF_NULL(parsed_line->extern_name, LINE_PARSER__PARSE_LINE__ALLOCATION_ERROR);
        (void)memcpy(parsed_line->extern_name, extern_name, strlen(extern_name));
    } 
    else 
    {
        parsed_line->line_type = LINE_TYPE__INSTRUCTION;
        opcode_end = strchr(line_pointer, ' ');

        /** No space character detected, reading all line */
        if (opcode_end == NULL)
        {
            opcode_end = strchr(line_pointer, '\0');
        }
        opcode_length = (int)(opcode_end - line_pointer);

        if (opcode_length > MAX_OPCODE_LEN)
        {
            return_code = LINE_PARSER__PARSE_LINE__TOO_LONG_OPCODE;
            goto Exit;
        }

        parsed_line->opcode = malloc(opcode_length + NULL_TERMINATOR_SIZE);
        memset(parsed_line->opcode, '\0', opcode_length + NULL_TERMINATOR_SIZE);
        EXIT_IF_NULL(parsed_line->opcode, LINE_PARSER__PARSE_LINE__ALLOCATION_ERROR);
        (void)memset(parsed_line->opcode, '\0', opcode_length + NULL_TERMINATOR_SIZE);
        (void)memcpy(parsed_line->opcode, line_pointer, opcode_length);

        EXIT_ON_ERROR(ARCHITECTURE__is_opcode(parsed_line->opcode, &is_opcode), &return_code);
        if (!is_opcode)
        {
            return_code = LINE_PARSER__PARSE_LINE__INVALID_OPCDODE;
            goto Exit;
        }
        line_pointer += opcode_length + 1;

        EXIT_ON_ERROR(line_parser__parse_instruction_operands(parsed_line->opcode, line_pointer, parsed_line), &return_code);
    }


    return_code = SUCCESS;
Exit:
    return return_code;
}