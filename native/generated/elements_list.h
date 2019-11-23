#pragma once

#include <stddef.h>

#include "minic/elements/element.h"

struct ElementsList;

struct ElementsListElement;

struct ElementsList * ElementsList_create();

void ElementsList_delete(struct ElementsList * instance);

size_t ElementsList_size(const struct ElementsList * self);

void ElementsList_append(struct ElementsList * self, const struct Elements *value);

void ElementsList_prepend(struct ElementsList * self, const struct Elements *value);

struct Elements * ElementsList_to_array(const struct ElementsList * self);

struct ElementsListElement * ElementsList_begin(const struct ElementsList * self);

struct ElementsListElement * ElementsList_end(const struct ElementsList * self);

struct ElementsListElement * ElementsListIterator_next(struct ElementsListElement * iterator);

struct ElementsListElement * ElementsListIterator_previous(struct ElementsListElement * iterator);

const struct Elements * ElementsListIterator_get(struct ElementsListElement * value);
