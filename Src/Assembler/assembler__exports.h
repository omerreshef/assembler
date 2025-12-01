#pragma once

#define MAX_SYMBOLS 1024
#define MAX_ENTRIES 50
#define MAX_SYMBOL_ATTRIBUTES 2

typedef enum {
    SYMBOL_CODE,
    SYMBOL_DATA,
    SYMBOL_EXTERNAL,
    SYMBOL_ENTRY
} symbol_attribute_t;

typedef enum {
    OPERAND_UNINITIALIZED,
    OPERAND_REGISTER,
    OPERAND_LABEL,
    OPERAND_IMMEDIATE,
    OPERAND_EXTERNAL,
    OPERAND_RELATIVE
} operand_type_t;

typedef struct {
    char *symbol_name;
    int ic_value;
    symbol_attribute_t attributes[MAX_SYMBOL_ATTRIBUTES];
} symbol_t;

typedef struct {
    symbol_t symbols[MAX_SYMBOLS];
} symbol_table_t;