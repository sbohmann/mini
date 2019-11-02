#pragma once

#include <stddef.h>

struct ReferenceCount {
    size_t strong_count;
    size_t *weak_count;
};

struct ComplexValue {
    struct ReferenceCount *reference_count;
};

void retain(struct ComplexValue *instance);
void release(struct ComplexValue *instance);

struct Any {
    char complex;
    union {
        char primitive_value[8];
        struct ComplexValue * complex_value;
    } value;
};

struct Position {
    const char *path;
    const size_t line;
    const size_t column;
};

struct Token {
    const char *text;
    const struct Any value;
    const struct Position position;
};

struct Tokens
{
    const char *path;
    const char *source;
};

struct Tokens * read_file(const char *path);
