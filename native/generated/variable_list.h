#pragma once

#include <stddef.h>

#include "minic/any.h"

struct VariableList;

struct VariableListElement;

struct VariableList * VariableList_create();

void VariableList_delete(struct VariableList * instance);

size_t VariableList_size(struct VariableList * self);

void VariableList_append(struct VariableList * self, struct Variable *value);

void VariableList_prepend(struct VariableList * self, struct Variable *value);

struct Variable * VariableList_to_array(struct VariableList * self);

struct VariableListElement * VariableList_begin(struct VariableList * self);

struct VariableListElement * VariableList_end(struct VariableList * self);

struct VariableListElement * VariableListIterator_next(struct VariableListElement * iterator);

struct VariableListElement * VariableListIterator_previous(struct VariableListElement * iterator);

struct Variable * VariableListIterator_get(struct VariableListElement * value);
