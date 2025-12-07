#include <string.h>

#include "second_pass__exports.h"
#include "text_editor__exports.h"
#include "common__exports.h"
#include "mcro_reader__exports.h"
#include "line_parser__exports.h"
#include "first_pass__exports.h"


int main(int argc, char *argv[])
{
    RC_t return_code = UNINITIALIZED;
    int instruction_counter = 0;
    int data_counter = 100;
    parsed_lines_t parsed_lines = {0};
    symbol_table_t symbol_table = {0};
    program_entries_t entries_list = {0};
    extern_usages_t extern_usages = {0};
    encoded_lines_t encoded_lines = {0};

    EXIT_ON_ERROR(TEXT_EDITOR__remove_spaces_and_tabs_in_given_path("/home/local/Projects/Maabada/assembler/text.txt", "/home/local/Projects/Maabada/assembler/text.txt.cpy"), &return_code);
    EXIT_ON_ERROR(MCRO_READER__convert_mcros_to_instructions("/home/local/Projects/Maabada/assembler/text.txt.cpy", "/home/local/Projects/Maabada/assembler/text_converted.txt"), &return_code);
    EXIT_ON_ERROR(FIRST_PASS__process_input_file("/home/local/Projects/Maabada/assembler/text_converted.txt", &instruction_counter, &data_counter, &parsed_lines, &symbol_table, &entries_list), &return_code);
    EXIT_ON_ERROR(SECOND_PASS__process(&parsed_lines, &symbol_table, &entries_list, &extern_usages, &encoded_lines), &return_code);

    return_code = SUCCESS;
Exit:
    return return_code;
}