#pragma once

#include <core/allocate.h>
#include <core/complex.h>
#include "any.h"

struct List {
    struct ComplexValue base;
    struct Any *data;
    size_t capacity;
    size_t size;
};

struct List *List_create(void);

struct List *List_copy(const struct List *other);

struct List *List_concatenate(const struct List *lhs, const struct List *rhs);

void List_add(struct List *self, struct Any value);

struct Any List_get(const struct List *self, size_t index);

void List_set(struct List *self, size_t index, struct Any value);
