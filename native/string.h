#pragma once

#include <stddef.h>

struct String {
    size_t length;
    const char * value;
};

struct StringList {
    size_t size;
    const struct String * value;
    struct StringList *next;
    struct StringList *previous;
};

struct StringList * StringList_create();
struct StringList * StringList_prepend(struct StringList * list, const struct String * value);
struct StringList * StringList_free(struct StringList * list);
