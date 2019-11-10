#pragma once

#include "string.h"

struct StringBuilder;

struct StringBuilder *StringBuilder_create();

struct StringBuilder *StringBuilder_delete(struct StringBuilder *self);

size_t StringBuilder_length(struct StringBuilder *self);

char StringBuilder_char_at(struct StringBuilder *self, size_t index);

struct StringBuilder *StringBuilder_append(struct StringBuilder *self, char c);

struct StringBuilder *StringBuilder_append_string(struct StringBuilder *self, const struct String *value);

struct String * StringBuilder_result(struct StringBuilder *self);
