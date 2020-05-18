#pragma once

#include <stddef.h>

#include "minic/elements/element.h"

struct ElementList;

struct ElementListElement;

struct ElementList * ElementList_create(void);

void ElementList_delete(struct ElementList * instance);

size_t ElementList_size(const struct ElementList * self);

void ElementList_append(struct ElementList * self, const struct Element *value);

void ElementList_prepend(struct ElementList * self, const struct Element *value);

struct Element * ElementList_to_array(const struct ElementList * self);

struct ElementListElement * ElementList_begin(const struct ElementList * self);

struct ElementListElement * ElementList_end(const struct ElementList * self);

struct ElementListElement * ElementListIterator_next(struct ElementListElement * iterator);

struct ElementListElement * ElementListIterator_previous(struct ElementListElement * iterator);

const struct Element * ElementListIterator_get(struct ElementListElement * value);
