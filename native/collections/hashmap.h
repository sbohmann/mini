#pragma once

#include <stddef.h>
#include <minic/any.h>
#include <core/complex.h>

#include "map_result.h"

struct HashMap {
    struct ComplexValue base;
    size_t size;
    struct Node *root;
};

typedef struct Any Key;
typedef struct Any Value;
typedef void (*HashMapForeachFunction) (Key key, Value value, void *context);

struct HashMap * HashMap_create(void);

void HashMap_release(struct HashMap *instance);

void HashMap_put(struct HashMap *self, Key key, Value value);

bool HashMap_set(struct HashMap *self, Key key, Value value);

struct MapResult HashMap_get(struct HashMap *self, Key key);

bool HashMap_remove(struct HashMap *self, Key key);

void HashMap_foreach(struct HashMap *self, HashMapForeachFunction, void *context);
