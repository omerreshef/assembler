#include <string.h>

#include "common__exports.h"
#include "architecture__internals.h"
#include "architecture__exports.h"

opcode_t OPCODES[] = {
    {"stop", 0},
    {"rts",  0},
    {"jmp",  1},
    {"prn",  1},
    {"red",  1},
    {"inc",  1},
    {"clr",  1},
    {"bne",  1},
    {"dec",  1},
    {"jsr",  1},
    {"not",  1},
    {"mov",  2},
    {"cmp",  2},
    {"add",  2},
    {"sub",  2},
    {"lea",  2}
};

register_t REGISTERS[] = {
    {"r0", 1},
    {"r1", 2},
    {"r2", 4},
    {"r3", 8},
    {"r4", 16},
    {"r5", 32},
    {"r6", 64},
    {"r7", 128}
};

incstruction_t INSTRUCTIONS[] = {
    {".string"},
    {".data"},
    {".entry"},
    {".extern"}
};

RC_t ARCHITECTURE__is_instruction(char *name, bool *is_instruction)
{
    RC_t return_code = UNINITIALIZED;
    int i = 0;

    if (name == NULL || is_instruction == NULL)
    {
        return_code = ARCHITECTURE__IS_INSTRUCTION__NULL_ARGUMENT;
        goto Exit;
    }

    for (i = 0; i < sizeof(INSTRUCTIONS) / sizeof(incstruction_t); i++)
    {
        if (strcmp(name, INSTRUCTIONS[i].instruction_name) == 0)
        {
            *is_instruction = true;
            return_code = SUCCESS;
            goto Exit;
        }
    }

    *is_instruction = false;
    
    return_code = SUCCESS;
Exit:
    return return_code;
}

RC_t ARCHITECTURE__is_opcode(char *name, bool *is_opcode)
{
    RC_t return_code = UNINITIALIZED;
    int i = 0;

    if (name == NULL || is_opcode == NULL)
    {
        return_code = ARCHITECTURE__IS_OPCODE__NULL_ARGUMENT;
        goto Exit;
    }

    for (i = 0; i < sizeof(OPCODES) / sizeof(opcode_t); i++)
    {
        if (strcmp(name, OPCODES[i].opcode_name) == 0)
        {
            *is_opcode = true;
            return_code = SUCCESS;
            goto Exit;
        }
    }

    *is_opcode = false;
    
    return_code = SUCCESS;
Exit:
    return return_code;
}

RC_t ARCHITECTURE__is_register(char *name, bool *is_register)
{
    RC_t return_code = UNINITIALIZED;
    int i = 0;

    if (name == NULL || is_register == NULL)
    {
        return_code = ARCHITECTURE__IS_REGISTER__NULL_ARGUMENT;
        goto Exit;
    }

    for (i = 0; i < sizeof(REGISTERS) / sizeof(register_t); i++)
    {
        if (strcmp(name, REGISTERS[i].register_name) == 0)
        {
            *is_register = true;
            return_code = SUCCESS;
            goto Exit;
        }
    }

    *is_register = false;
    
    return_code = SUCCESS;
Exit:
    return return_code;
}

RC_t ARCHITECTURE__get_register_value(char *name, int *register_value)
{
    RC_t return_code = UNINITIALIZED;
    int i = 0;

    if (name == NULL || register_value == NULL)
    {
        return_code = ARCHITECTURE__GET_REGISTER_VALUE__NULL_ARGUMENT;
        goto Exit;
    }

    for (i = 0; i < sizeof(REGISTERS) / sizeof(register_t); i++)
    {
        if (strcmp(name, REGISTERS[i].register_name) == 0)
        {
            *register_value = REGISTERS[i].binary_representation;
            return_code = SUCCESS;
            goto Exit;
        }
    }

    return_code = ARCHITECTURE__GET_REGISTER_VALUE__INVALID_REGISTER_MAME;
Exit:
    return return_code;
}

RC_t ARCHITECTURE__get_opcode_operands_number(char *opcode_name, int *operands_number)
{
    RC_t return_code = UNINITIALIZED;
    bool is_opcode = false;
    int i = 0;

    if (opcode_name == NULL || operands_number == NULL)
    {
        return_code = ARCHITECTURE__GET_OPCODE_OPERANDS_NUMBER__NULL_ARGUMENT;
        goto Exit;
    }

    EXIT_ON_ERROR(ARCHITECTURE__is_opcode(opcode_name, &is_opcode), &return_code);
    if (!is_opcode)
    {
        return_code = ARCHITECTURE__GET_OPCODE_OPERANDS_NUMBER__NOT_AN_OPCODE;
        goto Exit;
    }

    for (i = 0; i < sizeof(OPCODES) / sizeof(opcode_t); i++)
    {
        if (strcmp(opcode_name, OPCODES[i].opcode_name) == 0)
        {
            *operands_number = OPCODES[i].opcode_operands_number;
            break;
        }
    }

    return_code = SUCCESS;
Exit:
    return return_code;

}

RC_t ARCHITECTURE__get_opcode_size(char *opcode_name, int *opcode_size)
{
    RC_t return_code = UNINITIALIZED;
    bool is_opcode = false;
    int operands_number = 0;
    int i = 0;

    if (opcode_name == NULL || opcode_size == NULL)
    {
        return_code = ARCHITECTURE__GET_OPCODE_SIZE__NULL_ARGUMENT;
        goto Exit;
    }

    EXIT_ON_ERROR(ARCHITECTURE__get_opcode_operands_number(opcode_name, &operands_number), &return_code);

    *opcode_size = operands_number + 1; /* +1 for the opcode itself */

    return_code = SUCCESS;
Exit:
    return return_code;
}
