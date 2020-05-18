#pragma once

#include <stddef.h>

struct VoidPointerList;

struct VoidPointerListElement;

struct VoidPointerList * VoidPointerList_create(void);

void VoidPointerList_delete(struct VoidPointerList * instance);

size_t VoidPointerList_size(const struct VoidPointerList * self);

void VoidPointerList_append(struct VoidPointerList * self, void * value);

void VoidPointerList_prepend(struct VoidPointerList * self, void * value);

void * * VoidPointerList_to_array(const struct VoidPointerList * self);

struct VoidPointerListElement * VoidPointerList_begin(const struct VoidPointerList * self);

struct VoidPointerListElement * VoidPointerList_end(const struct VoidPointerList * self);

struct VoidPointerListElement * VoidPointerListIterator_next(struct VoidPointerListElement * iterator);

struct VoidPointerListElement * VoidPointerListIterator_previous(struct VoidPointerListElement * iterator);

void * VoidPointerListIterator_get(struct VoidPointerListElement * value);
