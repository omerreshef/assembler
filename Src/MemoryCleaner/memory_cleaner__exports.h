#pragma once

#include "assembler__exports.h"
#include "line_parser__exports.h"
#include "second_pass__exports.h"

/**
 * This function frees all dynamically allocated memory used in the assembler data structures.
 * Param @param parsed_lines The parsed lines structure containing assembly line details.
 * Param @param encoded_lines The encoded lines structure containing machine code.
 * 
 * Even if some free failed, the function will continue to free other allocated memory.
 */
void MEMORY_CLEANER__clean_allocated_memory(parsed_lines_t *parsed_lines, encoded_lines_t *encoded_lines);