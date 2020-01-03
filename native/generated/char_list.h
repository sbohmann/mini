#pragma once

#include <stddef.h>

struct CharList;

struct CharListElement;

struct CharList * CharList_create();

void CharList_delete(struct CharList * instance);

size_t CharList_size(const struct CharList * self);

void CharList_append(struct CharList * self, char value);

void CharList_prepend(struct CharList * self, char value);

char * CharList_to_array(const struct CharList * self);

struct CharListElement * CharList_begin(const struct CharList * self);

struct CharListElement * CharList_end(const struct CharList * self);

struct CharListElement * CharListIterator_next(struct CharListElement * iterator);

struct CharListElement * CharListIterator_previous(struct CharListElement * iterator);

char CharListIterator_get(struct CharListElement * value);
