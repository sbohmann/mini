#include "string.h"
#include "allocate.h"

#include <string.h>

struct String *String_from_literal(const char *literal) {
    struct String *result = allocate(sizeof(struct String));
    result->length = strlen(literal);
    result->value = literal;
    return result;
}

struct String *String_from_buffer(char *buffer) {
    struct String *result = allocate(sizeof(struct String));
    result->length = strlen(buffer);
    char *value = allocate(result->length);
    memcpy(value, buffer, result->length);
    result->value = value;
    return result;
}

bool equal(const struct String *lhs, const char *rhs) {
    return strcmp(lhs->value, rhs) == 0;
}

bool equals(const struct String *lhs, const struct String *rhs) {
    return strcmp(lhs->value, rhs->value) == 0;
}
