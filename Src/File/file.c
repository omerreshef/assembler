#include <string.h>

#include "file__exports.h"

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

    if (file_pointer == NULL || buffer == NULL || buffer_size == 0)
    {
        return_code = FILE__READ_LINE__NULL_ARGUMENT;
        goto Exit;
    }

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

    /* Remove new-line character from the read line if exists. */
    buffer[strcspn(buffer, "\n")] = '\0';

    return_code = SUCCESS;
Exit:
    return return_code;
}
