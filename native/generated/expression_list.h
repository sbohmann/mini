#pragma once

#include <stddef.h>

#include "ast/expression.h"

struct ExpressionList;

struct ExpressionListElement;

struct ExpressionList * ExpressionList_create(void);

void ExpressionList_delete(struct ExpressionList * instance);

size_t ExpressionList_size(const struct ExpressionList * self);

void ExpressionList_append(struct ExpressionList * self, const struct Expression *value);

void ExpressionList_prepend(struct ExpressionList * self, const struct Expression *value);

struct Expression * ExpressionList_to_array(const struct ExpressionList * self);

struct ExpressionListElement * ExpressionList_begin(const struct ExpressionList * self);

struct ExpressionListElement * ExpressionList_end(const struct ExpressionList * self);

struct ExpressionListElement * ExpressionListIterator_next(struct ExpressionListElement * iterator);

struct ExpressionListElement * ExpressionListIterator_previous(struct ExpressionListElement * iterator);

const struct Expression * ExpressionListIterator_get(struct ExpressionListElement * value);
