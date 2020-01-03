#pragma once

#include <core/string.h>
#include <minic/any.h>

struct Any String_split(const struct String *value, const struct String *separator);

struct Any String_trim(const struct String *value);
