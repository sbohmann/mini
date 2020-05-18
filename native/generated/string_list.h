#pragma once

#include <stddef.h>

#include "core/string.h"

struct StringList;

struct StringListElement;

struct StringList * StringList_create(void);

void StringList_delete(struct StringList * instance);

size_t StringList_size(const struct StringList * self);

void StringList_append(struct StringList * self, const struct String *value);

void StringList_prepend(struct StringList * self, const struct String *value);

struct String * StringList_to_array(const struct StringList * self);

struct StringListElement * StringList_begin(const struct StringList * self);

struct StringListElement * StringList_end(const struct StringList * self);

struct StringListElement * StringListIterator_next(struct StringListElement * iterator);

struct StringListElement * StringListIterator_previous(struct StringListElement * iterator);

const struct String * StringListIterator_get(struct StringListElement * value);
