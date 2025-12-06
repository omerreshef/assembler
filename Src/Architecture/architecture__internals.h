#define REGISTER_NAME_MAX_LENGTH 4
#define INSTRUCTION_NAME_MAX_LENGTH 12


typedef struct register_t {
    char register_name[REGISTER_NAME_MAX_LENGTH];
    int binary_representation;
} register_t;

typedef struct incstruction_t {
    char instruction_name[INSTRUCTION_NAME_MAX_LENGTH];
} incstruction_t;
