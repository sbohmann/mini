#pragma once

#include "minic/elements/element.h"

struct ElementQueue;

struct ElementQueue *ElementQueue_create(const struct Elements *list);

void ElementQueue_delete(struct ElementQueue *instance);

const struct Element *ElementQueue_peek(struct ElementQueue *self);

const struct Element *ElementQueue_next(struct ElementQueue *self);