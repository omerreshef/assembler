#include <string.h>

#include "file__exports.h"

RC_t FILE__is_exists(const char *file_path, bool *exists)
{
    RC_t return_code = UNINITIALIZED;
    FILE *file_pointer = NULL;

    if (file_path == NULL || exists == NULL)
    {
        return_code = FILE__IS_EXISTS__NULL_ARGUMENT;
        goto Exit;
    }

    /* If the file is opened, it exists. */
    file_pointer = fopen(file_path, "r");
    if (file_pointer != NULL)
    {
        *exists = true;
    }
    else
    {
        *exists = false;
    }

    return_code = SUCCESS;
Exit:
    if (file_pointer != NULL)
    {
        fclose(file_pointer);
    }
    return return_code;
}

RC_t FILE__open(const char *file_path, FILE **file_pointer, char *mode)
{
    RC_t return_code = UNINITIALIZED;

    if (file_path == NULL || file_pointer == NULL || mode == NULL)
    {
        return_code = FILE__OPEN__NULL_ARGUMENT;
        goto Exit;
    }

    *file_pointer = fopen(file_path, mode);
    if (*file_pointer == NULL)
    {
        return_code = FILE__OPEN__FOPEN_ERROR;
        goto Exit;
    }

    return_code = SUCCESS;
Exit:
    return return_code;
}

RC_t FILE__close(FILE *file_pointer)
{
    RC_t return_code = UNINITIALIZED;

    if (file_pointer == NULL)
    {
        return_code = FILE__CLOSE__NULL_ARGUMENT;
        goto Exit;
    }

    fclose(file_pointer);
    return_code = SUCCESS;
Exit:
    return return_code;
}

RC_t FILE__read_line(FILE *file_pointer, char *buffer, size_t buffer_size)
{
    RC_t return_code = UNINITIALIZED;
    size_t line_length = 0;
    int character = 0;

    if (file_pointer == NULL || buffer == NULL || buffer_size == 0)
    {
        return_code = FILE__READ_LINE__NULL_ARGUMENT;
        goto Exit;
    }

    /* Read a single line, handle a case of error */
    if (fgets(buffer, buffer_size, file_pointer) == NULL)
    {
        if (feof(file_pointer))
        {
            return_code = FILE__READ_LINE__EOF_REACHED;
        }
        else
        {
            return_code = FILE__READ_LINE__FGETS_ERROR;
        }
        goto Exit;
    }

    /* If the buffer was completely filled and there's no '\n', the line
     * was longer than the provided buffer (truncated by fgets). Treat this as
     * an error: consume the rest of the line from the stream and return
     * FILE__READ_LINE__LINE_TOO_LONG.
     */
    line_length = strlen(buffer);
    if (line_length == buffer_size - 1 && buffer[line_length - 1] != '\n')
    {
        /* consume remainder of the long line to position the stream at the
         * next line boundary. */
        character = 0;
        character = fgetc(file_pointer);
        while (character != EOF && character != '\n')
        {
            character = fgetc(file_pointer);
        }
        printf("Error - line '%s' too long to read into buffer of size %ld\n", buffer, buffer_size);
        return_code = FILE__READ_LINE__LINE_TOO_LONG;
        goto Exit;
    }

    /* Remove new-line character from the read line if exists. */
    buffer[strcspn(buffer, "\n")] = '\0';

    return_code = SUCCESS;
Exit:
    return return_code;
}

RC_t FILE__delete(const char *file_path)
{
    RC_t return_code = UNINITIALIZED;

    if (file_path == NULL)
    {
        return_code = FILE__DELETE__NULL_ARGUMENT;
        goto Exit;
    }

    if (remove(file_path) != 0)
    {
        return_code = FILE__DELETE__REMOVE_ERROR;
        goto Exit;
    }

    return_code = SUCCESS;
Exit:
    return return_code;
}
