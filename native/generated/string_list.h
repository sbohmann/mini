#pragma once

#include <stddef.h>

#include "core/string.h"

struct StringList;

struct StringListElement;

struct StringList * StringList_create();

struct StringList * StringList_delete(struct StringList * instance);

size_t StringList_size(struct StringList * self);

void StringList_append(struct StringList * self, struct String *value);

void StringList_prepend(struct StringList * self, struct String *value);

struct String * StringList_to_array(struct StringList * self);

struct StringListElement * StringList_begin(struct StringList * self);

struct StringListElement * StringList_end(struct StringList * self);

struct StringListElement * StringListIterator_next(struct StringListElement * iterator);

struct StringListElement * StringListIterator_previous(struct StringListElement * iterator);

struct String * StringListIterator_get(struct StringListElement * value);
