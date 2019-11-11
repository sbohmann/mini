#include "string.h"

#include <string.h>

bool equal(const struct String *lhs, const char *rhs) {
    return strcmp(lhs->value, rhs) == 0;
}

bool equals(const struct String *lhs, const struct String *rhs) {
    return strcmp(lhs->value, rhs->value) == 0;
}
