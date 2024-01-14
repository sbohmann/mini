#pragma once

#include <stddef.h>

#include "ast/statement.h"

struct StatementList;

struct StatementListElement;

struct StatementList * StatementList_create(void);

void StatementList_delete(struct StatementList * instance);

size_t StatementList_size(const struct StatementList * self);

void StatementList_append(struct StatementList * self, const struct Statement *value);

void StatementList_prepend(struct StatementList * self, const struct Statement *value);

struct Statement * StatementList_to_array(const struct StatementList * self);

struct StatementListElement * StatementList_begin(const struct StatementList * self);

struct StatementListElement * StatementList_end(const struct StatementList * self);

struct StatementListElement * StatementListIterator_next(struct StatementListElement * iterator);

struct StatementListElement * StatementListIterator_previous(struct StatementListElement * iterator);

const struct Statement * StatementListIterator_get(struct StatementListElement * value);
