#pragma once

#include <stddef.h>

#include <core/complex.h>
#include <minic/any.h>

struct HashSet {
    struct ComplexValue base;
    size_t size;
    struct Node *root;
};

struct HashSet;

typedef struct Any Element;

struct HashSet * HashSet_create(void);

void HashSet_release(struct HashSet *instance);

void HashSet_add(struct HashSet *self, Element element);

bool HashSet_contains(struct HashSet *self, Element element);

bool HashSet_remove(struct HashSet *self, Element element);
