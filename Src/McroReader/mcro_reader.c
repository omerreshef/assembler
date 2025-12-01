#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "file__exports.h"
#include "architecture__exports.h"

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

RC_t mcro_reader__read_mcros_from_file(const char *file_path, mcro_t *mcros)
{
    RC_t return_code = UNINITIALIZED;
    FILE *file_pointer = NULL;
    mcro_t *mcro_ptr = NULL;
    char line_buffer[MAX_LINE_LENGTH] = {0};
    char mcro_name[MAX_LINE_LENGTH] = {0};
    char *mcro_body_ptr = NULL;
    int current_mcro_index = 0;
    bool is_instruction = false;
    bool is_opcode = false;
    bool is_register = false;

    if (file_path == NULL)
    {
        return_code = MCRO_READER__READ_MCROS_FROM_FILE__NULL_ARGUMENT;
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
            /* Found the start of a macro definition */

            if (line_buffer[strlen(MCRO_START_DEFINITION)] != ' ')
            {
                /* Invalid macro definition line */
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
                return_code = MCRO_READER__INVALID_MCRO_NAME;
                goto Exit;
            }

            mcro_ptr = &mcros[current_mcro_index];
            strncpy(mcro_ptr->name, mcro_name, MAX_MCRO_NAME_LENGTH - 1);
            mcro_body_ptr = mcro_ptr->body;
            memset(mcro_body_ptr, 0, MAX_MCRO_LENGTH);
            
            if (current_mcro_index >= MAX_MCROS_COUNT)
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

                strncpy(mcro_body_ptr + strlen(mcro_body_ptr), line_buffer, MAX_MCRO_LENGTH - strlen(mcro_body_ptr) - 1);
                strncat(mcro_body_ptr, "\n", MAX_MCRO_LENGTH - strlen(mcro_body_ptr) - 1);

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

RC_t MCRO_READER__convert_mcros_to_instructions(const char *input_file_path, const char *output_file_path)
{
    RC_t return_code = UNINITIALIZED;
    FILE *input_file_pointer = NULL;
    FILE *output_file_pointer = NULL;
    mcro_t mcros[MAX_MCROS_COUNT] = {0};
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
        for (i = 0; i < MAX_MCROS_COUNT; i++)
        {
            if (strlen(mcros[i].name) == 0)
            {
                continue;
            }

            /* If the current line matches a macro name, copy its body to the output */
            if (strncmp(line_buffer, mcros[i].name, MAX_MCRO_NAME_LENGTH) == 0)
            {
                fprintf(output_file_pointer, "%s", mcros[i].body);
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
