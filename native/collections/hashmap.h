#pragma once

#include <stddef.h>
#include <minic/types.h>

struct HashMap;

typedef size_t Key;
typedef struct Any Value;

struct HashMap * HashMap_create();

void HashMap_delete(struct HashMap *instance);

void HashMap_put(struct HashMap *self, Key key, Value value);

Value HashMap_get(struct HashMap *self, Key key);

bool HashMap_remove(struct HashMap *self, Key key);
