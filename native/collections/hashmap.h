#pragma once

#include <stddef.h>
#include <minic/any.h>

#include "map_result.h"

struct HashMap;

typedef struct Any Key;
typedef struct Any Value;

struct HashMap * HashMap_create();

void HashMap_release(struct HashMap *instance);

void HashMap_put(struct HashMap *self, Key key, Value value);

bool HashMap_set(struct HashMap *self, Key key, Value value);

struct MapResult HashMap_get(struct HashMap *self, Key key);

bool HashMap_remove(struct HashMap *self, Key key);
