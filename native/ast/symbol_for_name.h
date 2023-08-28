#pragma once

#include <stddef.h>

#include "symbol.h"

struct Replacaments;

struct SymbolForName * SymbolForName_create();

void SymbolForName_delete(struct SymbolForName *instance);

void SymbolForName_add(struct SymbolForName *self, const char *key, const char *value);

struct Symbol SymbolForName_get(struct SymbolForName *self, const char *key);
