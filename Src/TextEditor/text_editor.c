#include <string.h>

#include "file__exports.h"

#include "text_editor__internals.h"
#include "text_editor__exports.h"

RC_t text_editor__remove_spaces_and_tabs_near_comma(char *input_line, char *output_line)
{
    size_t input_index = 0, output_index = 0;

    if (input_line == NULL || output_line == NULL)
        return TEXT_EDITOR__REMOVE_SPACES_AND_TABS_NEAR_COMMA__NULL_ARGUMENT;

    while (input_line[input_index] != '\0')
    {
        /* Skip spaces and tabs before comma */
        if ((input_line[input_index] == ' ' || input_line[input_index] == '\t') && input_line[input_index + 1] == ',')
        {
            input_index++;
            continue;
        }

        /* Skip spaces and tabs after comma */
        if (input_line[input_index] == ',')
        {
            output_line[output_index] = ',';
            output_index++;
            input_index++;

            while (input_line[input_index] == ' ' || input_line[input_index] == '\t')
            {
                input_index++;
            }
            continue;
        }

        output_line[output_index] = input_line[input_index];
        input_index++;
        output_index++;
    }

    output_line[output_index] = '\0';
    return SUCCESS;
}

RC_t text_editor__remove_spaces_and_tabs_from_line(char *input_line, char *output_line)
{
    RC_t return_code = UNINITIALIZED;
    size_t input_index = 0;
    size_t output_index = 0;
    char temporarty_line[MAX_LINE_LENGTH] = {0};

    if (input_line == NULL || output_line == NULL)
    {
        return_code = TEXT_EDITOR__REMOVE_SPACES_AND_TABS_FROM_LINE__NULL_ARGUMENT;
        goto Exit;
    }

    /* Skip spaces and tabs at the beginning */
    while (input_line[input_index] == ' ' || input_line[input_index] == '\t')
    {
        input_index++;
    }

    while (input_line[input_index] != '\0')
    {
        /* Copy non-space/tab characters */
        while 
        (
            input_line[input_index] != ' ' && 
            input_line[input_index] != '\t' &&
            input_line[input_index] != '\0'
        )
        {
            output_line[output_index] = input_line[input_index];
            output_index++;
            input_index++;
        }

        /* If reached end of line, break */
        if (input_line[input_index] == '\0')
        {
            break;
        }

        /* Skip spaces and tabs */
        while (input_line[input_index] == ' ' || input_line[input_index] == '\t')
        {
            input_index++;
        }

        /* Add a single space to separate words, if not at the end of the line */
        if (input_line[input_index] != '\0')
        {
            output_line[output_index] = ' ';
            output_index++;
        }
    }
    output_line[output_index] = '\0';

    EXIT_ON_ERROR(text_editor__remove_spaces_and_tabs_near_comma(output_line, temporarty_line), &return_code);
    (void)strncpy(output_line, temporarty_line, MAX_LINE_LENGTH);

    return_code = SUCCESS;
Exit:
    return return_code;
}

RC_t text_editor__remove_spaces_and_tabs_from_file(FILE *input_file, FILE *output_file)
{
    RC_t return_code = UNINITIALIZED;
    char line_buffer[MAX_LINE_LENGTH];
    char modified_line[MAX_LINE_LENGTH];

    if (input_file == NULL || output_file == NULL)
    {
        return_code = TEXT_EDITOR__REMOVE_SPACES_AND_TABS_FROM_FILE__NULL_ARGUMENT;
        goto Exit;
    }

    while (1)
    {
        /* Clear line buffer before reading */
        memset(line_buffer, 0, sizeof(line_buffer));
        return_code = FILE__read_line(input_file, line_buffer, sizeof(line_buffer));

        if (return_code == FILE__READ_LINE__EOF_REACHED)
        {
            break;
        }
        else if (return_code != SUCCESS)
        {
            goto Exit;
        }

        EXIT_ON_ERROR(text_editor__remove_spaces_and_tabs_from_line(line_buffer, modified_line), &return_code);

        fprintf(output_file, "%s\n", modified_line);
    }

    return_code = SUCCESS;
Exit:
    return return_code;
}

RC_t TEXT_EDITOR__remove_spaces_and_tabs_in_given_path(const char *input_file_path, char *new_file_path)
{
    RC_t return_code = UNINITIALIZED;
    FILE *input_file = NULL;
    FILE *output_file = NULL;

    if (input_file_path == NULL || new_file_path == NULL)
    {
        return_code = TEXT_EDITOR__REMOVE_SPACES_AND_TABS_IN_GIVEN_PATH__NULL_ARGUMENT;
        goto Exit;
    }

    EXIT_ON_ERROR(FILE__open(input_file_path, &input_file, "r"), &return_code);
    EXIT_ON_ERROR(FILE__open(new_file_path, &output_file, "w"), &return_code);

    EXIT_ON_ERROR(text_editor__remove_spaces_and_tabs_from_file(input_file, output_file), &return_code);

    return_code = SUCCESS;
Exit:
    if (input_file != NULL)
    {
        FILE__close(input_file);
    }
    if (output_file != NULL)
    {
        FILE__close(output_file);
    }
    return return_code;
}
