#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "file__exports.h"
#include "architecture__exports.h"
#include "line_parser__exports.h"

#include "mcro_reader__exports.h"
#include "mcro_reader__internals.h"

RC_t mcro_reader__extract_mcro_name(const char *line_buffer, char *mcro_name)
{
    RC_t return_code = UNINITIALIZED;
    size_t start_index = 0;
    size_t line_length = 0;
    size_t name_length = 0;

    if (line_buffer == NULL || mcro_name == NULL)
    {
        return_code = MCRO_READER__EXTRACT_MCRO_NAME__NULL_ARGUMENT;
        goto Exit;
    }

    start_index =  strlen(MCRO_START_DEFINITION) + 1; /* +1 for space after 'mcro' */
    line_length = strlen(line_buffer);
    name_length = line_length - start_index;

    if (name_length >= MAX_MCRO_NAME_LENGTH)
    {
        /* Macro name too long */
        return_code = MCRO_READER__INVALID_MCRO_START_DEFINITION;
        goto Exit;
    }

    strncpy(mcro_name, line_buffer + start_index, name_length);
    mcro_name[name_length] = '\0';

    return_code = SUCCESS;
Exit:
    return return_code;
}

RC_t mcro_reader__calculate_mcro_size(const char *file_path, const char *mcro_name, int *mcro_size)
{
    RC_t return_code = UNINITIALIZED;
    FILE *file_pointer = NULL;
    char line_buffer[MAX_LINE_LENGTH] = {0};
    bool is_macro_end = false;
    bool is_macro_start = false;
    int size = 0;

    if (file_path == NULL || mcro_name == NULL || mcro_size == NULL)
    {
        return_code = MCRO_READER__CALCULATE_MCRO_SIZE__NULL_ARGUMENT;
        goto Exit;
    }

    EXIT_ON_ERROR(FILE__open(file_path, &file_pointer, "r"), &return_code);

    while (1)
    {
        memset(line_buffer, 0, sizeof(line_buffer));
        return_code = FILE__read_line(file_pointer, line_buffer, sizeof(line_buffer));

        if (return_code == FILE__READ_LINE__EOF_REACHED)
        {
            break;
        }
        else if (return_code != SUCCESS)
        {
            goto Exit;
        }

        if (!is_macro_start)
        {
            /* Looking for the start of the macro definition */
            if (strncmp(line_buffer, MCRO_START_DEFINITION, strlen(MCRO_START_DEFINITION)) == 0)
            {
                char extracted_mcro_name[MAX_MCRO_NAME_LENGTH] = {0};
                EXIT_ON_ERROR(mcro_reader__extract_mcro_name(line_buffer, extracted_mcro_name), &return_code);
                if (strcmp(extracted_mcro_name, mcro_name) == 0)
                {
                    is_macro_start = true;
                }
            }
            continue; /* Continue to next line until macro start is found */
        }

        if (strncmp(line_buffer, MCRO_END_DEFINITION, strlen(MCRO_END_DEFINITION)) == 0)
        {
            is_macro_end = true;
            break;
        }
        if (is_macro_start)
        {
            /* Counting lines within the macro definition */
            size += strlen(line_buffer) + 1; /* +1 for newline character */
        }
    }

    if (!is_macro_end)
    {
        return_code = MCRO_READER__CALCULATE_MCRO_SIZE__MACRO_NOT_FOUND;
        goto Exit;
    }

    *mcro_size = size + NULL_TERMINATOR_SIZE;

Exit:
    if (file_pointer != NULL)
    {
        FILE__close(file_pointer);
    }
    
    return return_code;
}

RC_t mcro_reader__count_macros_amount(const char *file_path, int *mcros_amount)
{
    RC_t return_code = UNINITIALIZED;
    FILE *file_pointer = NULL;
    char line_buffer[MAX_LINE_LENGTH] = {0};
    int count = 0;

    if (file_path == NULL || mcros_amount == NULL)
    {
        return_code = MCRO_READER__COUNT_MACROS_AMOUNT__NULL_ARGUMENT;
        goto Exit;
    }

    EXIT_ON_ERROR(FILE__open(file_path, &file_pointer, "r"), &return_code);

    while (1)
    {
        memset(line_buffer, 0, sizeof(line_buffer));
        return_code = FILE__read_line(file_pointer, line_buffer, sizeof(line_buffer));

        if (return_code == FILE__READ_LINE__EOF_REACHED)
        {
            break;
        }
        else if (return_code != SUCCESS)
        {
            goto Exit;
        }

        if (strncmp(line_buffer, MCRO_START_DEFINITION, strlen(MCRO_START_DEFINITION)) == 0)
        {
            count++;
        }
    }

    *mcros_amount = count/2; /* each macro has a start and end definition */
    return_code = SUCCESS;
Exit:
    if (file_pointer != NULL)
    {
        FILE__close(file_pointer);
    }
    return return_code;
}

RC_t mcro_reader__read_mcros_from_file(const char *file_path, mcros_collection_t *mcros)
{
    RC_t return_code = UNINITIALIZED;
    FILE *file_pointer = NULL;
    mcro_t *mcro_ptr = NULL;
    char line_buffer[MAX_LINE_LENGTH] = {0};
    char mcro_name[MAX_LINE_LENGTH] = {0};
    char *mcro_body_ptr = NULL;
    bool is_instruction = false;
    bool is_opcode = false;
    bool is_register = false;
    int current_mcro_index = 0;
    int mcros_amount = 0;
    int mcro_size = 0;

    if (file_path == NULL)
    {
        return_code = MCRO_READER__READ_MCROS_FROM_FILE__NULL_ARGUMENT;
        goto Exit;
    }

    EXIT_ON_ERROR(mcro_reader__count_macros_amount(file_path, &mcros_amount), &return_code);
    mcros->mcros_amount = mcros_amount;
    mcros->mcros = malloc(sizeof(mcro_t) * mcros_amount);
    EXIT_IF_NULL(mcros->mcros, MCRO_READER__READ_MCROS_FROM_FILE__ALLOCATION_FAILED);
    memset(mcros->mcros, 0, sizeof(mcro_t) * mcros_amount);

    EXIT_ON_ERROR(FILE__open(file_path, &file_pointer, "r"), &return_code);

    while (1)
    {
        memset(line_buffer, 0, sizeof(line_buffer));
        return_code = FILE__read_line(file_pointer, line_buffer, sizeof(line_buffer));

        if (return_code == FILE__READ_LINE__EOF_REACHED)
        {
            break;
        }
        else if (return_code != SUCCESS)
        {
            goto Exit;
        }

        if (strncmp(line_buffer, MCRO_START_DEFINITION, strlen(MCRO_START_DEFINITION)) == 0)
        {
            /* Found the start of a macro definition */

            if (line_buffer[strlen(MCRO_START_DEFINITION)] != ' ')
            {
                /* Invalid macro definition line */
                printf("Invalid macro start definition: %s\n", line_buffer);
                return_code = MCRO_READER__INVALID_MCRO_START_DEFINITION;
                goto Exit;
            }

            memset(mcro_name, 0, MAX_MCRO_NAME_LENGTH);
            EXIT_ON_ERROR(mcro_reader__extract_mcro_name(line_buffer, mcro_name), &return_code);

            EXIT_ON_ERROR(ARCHITECTURE__is_instruction(mcro_name, &is_instruction), &return_code);
            EXIT_ON_ERROR(ARCHITECTURE__is_opcode(mcro_name, &is_opcode), &return_code);
            EXIT_ON_ERROR(ARCHITECTURE__is_register(mcro_name, &is_register), &return_code);

            if (is_instruction || is_opcode || is_register)
            {
                /* Macro name conflicts with existing instruction, opcode, or register */
                printf("Macro name conflicts with existing instruction, opcode, or register: %s\n", mcro_name);
                return_code = MCRO_READER__INVALID_MCRO_NAME;
                goto Exit;
            }

            mcro_ptr = &mcros->mcros[current_mcro_index];
            mcro_ptr->name = malloc(strlen(mcro_name) + NULL_TERMINATOR_SIZE);
            EXIT_IF_NULL(mcro_ptr->name, MCRO_READER__READ_MCROS_FROM_FILE__ALLOCATION_FAILED);
            memset(mcro_ptr->name, 0, strlen(mcro_name) + NULL_TERMINATOR_SIZE);
            strncpy(mcro_ptr->name, mcro_name, strlen(mcro_name));

            EXIT_ON_ERROR(mcro_reader__calculate_mcro_size(file_path, mcro_ptr->name, &mcro_size), &return_code);

            mcro_ptr->body_size = mcro_size;
            mcro_ptr->body = malloc(mcro_size);
            memset(mcro_ptr->body, 0, mcro_size);
            mcro_body_ptr = mcro_ptr->body;

            if (current_mcro_index >= mcros_amount)
            {
                /* Reached maximum number of macros */
                return_code = MCRO_READER__MAX_MCROS_EXCEEDED;
                goto Exit;
            }

            current_mcro_index++;

            memset(line_buffer, 0, sizeof(line_buffer));
            EXIT_ON_ERROR(FILE__read_line(file_pointer, line_buffer, sizeof(line_buffer)), &return_code);

            while (strncmp(line_buffer, MCRO_END_DEFINITION, strlen(MCRO_END_DEFINITION)) != 0)
            {
                /* Read macro body lines until "mcroend" is found */

                /* Macro end statement should be only "mcroend", but here it is not */
                if (strstr(line_buffer, MCRO_END_DEFINITION) != NULL)
                {
                    return_code = MCRO_READER__INVALID_MCRO_END_DEFINITION;
                    goto Exit;
                }

                strncpy(mcro_body_ptr + strlen(mcro_body_ptr), line_buffer, mcro_ptr->body_size - strlen(mcro_body_ptr) - 1);
                strncat(mcro_body_ptr, "\n", mcro_ptr->body_size - strlen(mcro_body_ptr) - 1);

                memset(line_buffer, 0, sizeof(line_buffer));
                EXIT_ON_ERROR(FILE__read_line(file_pointer, line_buffer, sizeof(line_buffer)), &return_code);
            }
        }
    }

    return_code = SUCCESS;
Exit:
    if (file_pointer != NULL)
    {
        FILE__close(file_pointer);
    }
    return return_code; 
}

RC_t MCRO_READER__convert_mcros_to_instructions(const char *input_file_path, const char *output_file_path, mcros_collection_t *mcros)
{
    RC_t return_code = UNINITIALIZED;
    FILE *input_file_pointer = NULL;
    FILE *output_file_pointer = NULL;
    char line_buffer[MAX_LINE_LENGTH] = {0};
    bool converted_mcro_to_instructions = false;
    bool is_inside_mcro = false;
    int i = 0;

    if (input_file_path == NULL || output_file_path == NULL)
    {
        return_code = MCRO_READER__CONVERT_MCROS_TO_INSTRUCTIONS__NULL_ARGUMENT;
        goto Exit;
    }

    EXIT_ON_ERROR(mcro_reader__read_mcros_from_file(input_file_path, mcros), &return_code);
    EXIT_ON_ERROR(FILE__open(input_file_path, &input_file_pointer, "r"), &return_code);
    EXIT_ON_ERROR(FILE__open(output_file_path, &output_file_pointer, "w"), &return_code);

    while (1)
    {
        memset(line_buffer, 0, sizeof(line_buffer));
        return_code = FILE__read_line(input_file_pointer, line_buffer, sizeof(line_buffer));
        if (return_code == FILE__READ_LINE__EOF_REACHED)
        {
            break;
        }
        else if (return_code != SUCCESS)
        {
            goto Exit;
        }
        
        /* Check if the current line is the end of a macro definition */
        if (strncmp(line_buffer, MCRO_END_DEFINITION, strlen(MCRO_END_DEFINITION)) == 0)
        {
            is_inside_mcro = false;
            continue;
        }
        
        /* Check if the current line is the start of a macro definition */
        if (strncmp(line_buffer, MCRO_START_DEFINITION, strlen(MCRO_START_DEFINITION)) == 0)
        {
            is_inside_mcro = true;
            continue;
        }

        /* If we inside mcro definition we should skip the line */
        if (is_inside_mcro)
        {
            continue;
        }

        /* Check if the current line matches any macro name */
        for (i = 0; i < mcros->mcros_amount; i++)
        {
            if (strlen(mcros->mcros[i].name) == 0)
            {
                continue;
            }

            /* If the current line matches a macro name, copy its body to the output */
            if (strncmp(line_buffer, mcros->mcros[i].name, MAX_MCRO_NAME_LENGTH) == 0)
            {
                fprintf(output_file_pointer, "%s", mcros->mcros[i].body);
                converted_mcro_to_instructions = true;
                break;
            }
        }

        /* Mcro already converted so we should skip to the next line */
        if (converted_mcro_to_instructions)
        {
            converted_mcro_to_instructions = false;
            continue;
        }

        /* If we reached here, its not a macro call and should be written as is */
        fprintf(output_file_pointer, "%s\n", line_buffer);

    }

    return_code = SUCCESS;

Exit:

    if (input_file_pointer != NULL)
    {
        FILE__close(input_file_pointer);
    }

    if (output_file_pointer != NULL)
    {
        FILE__close(output_file_pointer);
    }

    return return_code;
}
