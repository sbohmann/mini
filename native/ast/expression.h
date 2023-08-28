#pragma once

#include "Symbol.h"
#include "generated/expression_list.h"

struct Expression;

struct Expression functionCallExpression(struct Symbol *function, struct ExpressionList *arguments);
