#include "common__exports.h"

#define RC(name, desc) desc,
#define RC_LAST(name, desc) desc
static const char *RC_descriptions[] = {
    "UNINITIALIZED",
#include "rc.hx"
};
#undef RC
#undef RC_LAST

const char *RC_description(RC_t rc) {
    int idx = rc + 1; 
    if (idx < 0 || idx >= (int)(sizeof(RC_descriptions)/sizeof(RC_descriptions[0]))) {
        return "Unknown RC value";
    }
    return RC_descriptions[idx];
}
