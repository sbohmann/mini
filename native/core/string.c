#include "string.h"
#include "allocate.h"

#include <string.h>
#include <minic/any.h>
#include <stdlib.h>

const struct String *String_from_literal(const char *literal) {
    struct String *result = allocate(sizeof(struct String));
    result->length = strlen(literal);
    result->value = literal;
    result->hash = string_hash(result->value, result->length);
    result->is_literal = true;
    return result;
}

const struct String *String_preallocated(const char *value, size_t length) {
    struct String *result = allocate(sizeof(struct String));
    result->length = length;
    result->value = value;
    result->hash = string_hash(result->value, result->length);
    return result;
}

const struct String *String_from_buffer(char *buffer, size_t length) {
    struct String *result = allocate(sizeof(struct String));
    result->length = length;
    char *value = allocate_raw(length + 1);
    memcpy(value, buffer, length);
    value[length] = 0;
    result->value = value;
    result->hash = string_hash(result->value, result->length);
    return result;
}

bool String_equal(const struct String *lhs, const struct String *rhs) {
    return strcmp(lhs->value, rhs->value) == 0;
}

bool String_equal_to_literal(const struct String *lhs, const char *rhs) {
    return strcmp(lhs->value, rhs) == 0;
}

void String_pin(const struct String *instance) {
    // TODO mark in parser GC
}

void String_delete(struct String *instance) {
    if (!instance->is_literal) {
        deallocate((void *)instance->value);
    }
}
