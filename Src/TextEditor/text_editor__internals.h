#pragma once
#include <stdio.h>

#include "common__exports.h"


/**
 * Remove spaces and tabs near commas in input line and write the result to output line.
 * @input_line: null-terminated string to process.
 * @output_line: provided buffer where the modified line will be stored.
 * 
 * On success, writes the modified line into @output_line and returns SUCCESS.
 */
RC_t text_editor__remove_spaces_and_tabs_near_comma(char *input_line, char *output_line);

/**
 * Remove spaces and tabs from input line and write the result to output line.
 * @input_line: null-terminated string to process.
 * @output_line: provided buffer where the modified line will be stored.
 * 
 * On success, writes the modified line into @output_line and returns SUCCESS.
 */
RC_t text_editor__remove_spaces_and_tabs_from_line(char *input_line, char *output_line);

/**
 * Remove spaces and tabs from input file and write the result to output file.
 * @input_file: open FILE stream to read from.
 * @output_file: open FILE stream to write to.
 * 
 * On success, writes the modified lines into @output_file and returns SUCCESS.
 */
RC_t text_editor__remove_spaces_and_tabs_from_file(FILE *input_file, FILE *output_file);