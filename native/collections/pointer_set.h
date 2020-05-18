#pragma once

#include <stddef.h>
#include <stdbool.h>

struct PointerSet {
    size_t size;
    struct Node *root;
};

struct PointerSet;

typedef size_t Element;

struct PointerSet * PointerSet_create(void);

void PointerSet_delete(struct PointerSet *instance);

void PointerSet_add(struct PointerSet *self, Element element);

bool PointerSet_contains(struct PointerSet *self, Element element);

bool PointerSet_remove(struct PointerSet *self, Element element);
