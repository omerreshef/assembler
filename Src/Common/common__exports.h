#pragma once

typedef enum RC_t {
    UNINITIALIZED = -1,
#define RC(name, desc) name,
#define RC_LAST(name, desc) name
#include "rc.hx"
#undef RC
#undef RC_LAST
} RC_t;


#define EXIT_ON_ERROR(function_output, return_code) do {    \
    int rc = (function_output);                             \
    *(return_code) = rc;                                    \
    if (rc != 0) {                                          \
        goto Exit;                                          \
    }                                                       \
} while (0)

#define EXIT_IF_NULL(value, rc) do {                       \
    if (value == NULL) {                                   \
        return_code = rc;                                  \
        goto Exit;                                         \
    }                                                      \
} while (0)

#define MAX_LINE_LENGTH 100
#define MAX_FILE_PATH_LENGTH 256
#define MAX_LINES_IN_FILE 1024

/* Return a description string for a given RC_t value */
const char *RC_description(RC_t rc);
