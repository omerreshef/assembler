#pragma once
#include <stdio.h>

#include "common__exports.h"

/**
 * Remove spaces and tabs from the file at the given input path and
 * write the result to a new file whose path is stored in @new_file_path.
 * @input_file_path: path to the input file to process.
 * @new_file_path: buffer where the new file path will be written.
 * 
 * On success, writes the new file path into @new_file_path and returns SUCCESS.
 */
RC_t TEXT_EDITOR__remove_spaces_and_tabs_in_given_path(const char *input_file_path, char *new_file_path);