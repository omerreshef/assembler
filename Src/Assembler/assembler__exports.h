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
    OPERAND_UNINITIALIZED = -1,
    OPERAND_IMMEDIATE,
    OPERAND_DIRECT,
    OPERAND_RELATIVE,
    OPERAND_REGISTER,
    OPERAND_EXTERNAL
} operand_type_t;

typedef struct program_entry_t {
    char *entry_name;
} program_entry_t;

typedef struct program_entries_t {
    program_entry_t entries[MAX_ENTRIES];
} program_entries_t;

typedef struct {
    char *symbol_name;
    int ic_value;
    symbol_attribute_t attributes[MAX_SYMBOL_ATTRIBUTES];
} symbol_t;

typedef struct {
    symbol_t symbols[MAX_SYMBOLS];
} symbol_table_t;