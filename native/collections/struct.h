#pragma once

#include <stddef.h>
#include <minic/any.h>
#include <core/complex.h>

#include "map_result.h"

struct Struct {
    struct ComplexValue base;
    size_t size;
    struct Node *root;
};

typedef const struct String *Name;
typedef struct Any Value;

struct Struct * Struct_create();

void Struct_release(struct Struct *instance);

void Struct_put(struct Struct *self, Name name, Value value);

bool Struct_set(struct Struct *self, Name name, Value value);

struct MapResult Struct_get(struct Struct *self, Name name);

bool Struct_remove(struct Struct *self, Name name);
