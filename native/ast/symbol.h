#pragma once

#include <stddef.h>

struct Symbol {
    const char * const text;
    const size_t size;
};

struct Symbol * Symbol_create(const char *name);
