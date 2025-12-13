#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

#include "common__exports.h"

/**
 * Check if a file exists.
 * @file_path: path to the file to check.
 * @exists: pointer to a bool where the result will be stored.
 * 
 * On success, the function sets *exists to true if the file exists,
 * or false if it does not, and returns SUCCESS.
 */
RC_t FILE__is_exists(const char *file_path, bool *exists);

/**
 * Open a file.
 * @file_path: path to the file to open.
 * @file_pointer: pointer to the FILE stream object.
 * @mode: opening mode - read, write etc.
 */
RC_t FILE__open(const char *file_path, FILE **file_pointer, char *mode);

/**
 * Close a file
 * @file_pointer: FILE stream object to close.
 */
RC_t FILE__close(FILE *file_pointer);

/**
 * Read a single line from a file stream into a buffer.
 * @file_pointer: open FILE stream to read from.
 * @buffer: caller-provided buffer where the line will be stored.
 * @buffer_size: size of @buffer in bytes.
 *
 * On success the function writes the line (without the trailing '\n')
 * into @buffer and returns SUCCESS. If any pointer is NULL or
 * @buffer_size is 0, returns FILE__READ_LINE__NULL_ARGUMENT. If reading the
 * line fails (fgets returns NULL) returns FILE__READ_LINE__FGETS_ERROR.
 */
RC_t FILE__read_line(FILE *file_pointer, char *buffer, size_t buffer_size);

/**
 * Delete a file from the file system.
 * @file_path: path to the file to delete.
 * 
 * On success, the function deletes the file and returns SUCCESS.
 */
RC_t FILE__delete(const char *file_path);