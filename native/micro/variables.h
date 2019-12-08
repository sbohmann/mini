#pragma once

#include <core/string.h>
#include <minic/any.h>

struct Variables;

struct Variables *Variables_create(struct Variables *context);

void Variables_retain(struct Variables *instance);

void Variables_release(struct Variables *instance);

bool set_variable(struct Variables *, const struct String *name, struct Any value);

void create_variable(struct Variables *self, const struct String *name, struct Any value);

struct MapResult get_variable(const struct Variables *, const struct String *name);
