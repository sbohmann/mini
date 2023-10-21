#pragma once

#include <stddef.h>

#include "symbol.h"

struct Replacaments;

struct SymbolForName * SymbolForName_create(void);

void SymbolForName_delete(struct SymbolForName *instance);

struct Symbol * SymbolForName_get(struct SymbolForName *self, const char *key, const char *value);
