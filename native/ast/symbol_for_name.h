#pragma once

#include <stddef.h>

#include "symbol.h"

struct Replacaments;

struct SymbolForName * SymbolForName_create(void);

void SymbolForName_delete(struct SymbolForName *instance);

struct Symbol * SymbolForName_get(struct SymbolForName *self, const char *key);

void SymbolForName_set(struct SymbolForName *self, const char *key, const struct Symbol *value);

struct Symbol * SymbolForName_add(struct SymbolForName *self, const char *name);
