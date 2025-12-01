#include "text_editor__exports.h"
#include "common__exports.h"
#include "mcro_reader__exports.h"
#include "line_parser__exports.h"
#include "first_pass__exports.h"

#include <string.h>

int main(int argc, char *argv[])
{
    RC_t return_code = UNINITIALIZED;
    int instruction_counter = 0;
    int data_counter = 100;
    parsed_line_t parsed_lines[500] = {0};
    symbol_table_t symbol_table;
    char *entries_list[50];

    EXIT_ON_ERROR(TEXT_EDITOR__remove_spaces_and_tabs_in_given_path("/home/local/Projects/Maabada/assembler/text.txt", "/home/local/Projects/Maabada/assembler/text.txt.cpy"), &return_code);
    EXIT_ON_ERROR(MCRO_READER__convert_mcros_to_instructions("/home/local/Projects/Maabada/assembler/text.txt.cpy", "/home/local/Projects/Maabada/assembler/text_converted.txt"), &return_code);
    EXIT_ON_ERROR(FIRST_PASS__process_input_file("/home/local/Projects/Maabada/assembler/text_converted.txt", &instruction_counter, &data_counter, parsed_lines, &symbol_table, entries_list), &return_code);

    printf("opcode=%s %s,%s\n", parsed_lines[0].opcode, parsed_lines[0].operands[0], parsed_lines[0].operands[1]);
    printf("opcode=%s\n", parsed_lines[1].opcode);
    printf("opcode=%s\n", parsed_lines[2].opcode);
    printf("opcode=%s\n", parsed_lines[3].opcode);
    printf("opcode=%s\n", parsed_lines[4].opcode);
    printf("opcode=%s\n", parsed_lines[5].opcode);
    printf("opcode=%s\n", parsed_lines[6].opcode);
    printf("opcode=%s\n", parsed_lines[7].opcode);
    printf("opcode=%s\n", parsed_lines[8].opcode);

    return_code = SUCCESS;
Exit:
    return return_code;
}