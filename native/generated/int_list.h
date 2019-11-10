#pragma once

#include <stddef.h>
#include <stdint.h>

struct IntList;

struct IntListElement;

struct IntList * IntList_create();

void IntList_delete(struct IntList * instance);

size_t IntList_size(struct IntList * self);

void IntList_append(struct IntList * self, int64_t value);

void IntList_prepend(struct IntList * self, int64_t value);

int64_t * IntList_to_array(struct IntList * self);

struct IntListElement * IntList_begin(struct IntList * self);

struct IntListElement * IntList_end(struct IntList * self);

struct IntListElement * IntListIterator_next(struct IntListElement * iterator);

struct IntListElement * IntListIterator_previous(struct IntListElement * iterator);

int64_t IntListIterator_get(struct IntListElement * value);
