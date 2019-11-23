#pragma once

#include "expressions.h"

#include <stdbool.h>

#include <core/string.h>
#include <generated/element_queue.h>
#include <minic/elements/element.h>

bool equal(const struct String *string, const char *literal);

const char *element_text(const struct Element *element);

void read_comma(struct ElementQueue *elements);

void read_operator(struct ElementQueue *elements, const char *text);

const struct Token *read_token(struct ElementQueue *elements);

const struct String *read_symbol(struct ElementQueue *elements);

struct ElementQueue *read_paren_block(struct ElementQueue *elements);

struct ElementQueue *read_square_block(struct ElementQueue *elements);

struct ElementQueue *read_curly_block(struct ElementQueue *elements);
