#define OPCODE_NAME_MAX_LENGTH 8
#define REGISTER_NAME_MAX_LENGTH 4
#define INSTRUCTION_NAME_MAX_LENGTH 12

typedef struct opcode_t {
    char opcode_name[OPCODE_NAME_MAX_LENGTH];
    int opcode_operands_number;
} opcode_t;

typedef struct register_t {
    char register_name[REGISTER_NAME_MAX_LENGTH];
    int binary_representation;
} register_t;

typedef struct incstruction_t {
    char instruction_name[INSTRUCTION_NAME_MAX_LENGTH];
} incstruction_t;
