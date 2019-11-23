#pragma once

#include <core/string.h>
#include <minic/any.h>

void set_variable(const struct String *name, const struct String *text, struct Any value);

struct Any get_variable(const struct String *name);

void initialize_variables();
