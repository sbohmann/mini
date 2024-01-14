#pragma once

#include "symbol.h"
#include "generated/expression_list.h"

struct Assignment {
    struct Symbol *variable;
    struct Expression *value;
};

struct FunctionCall {
    struct Symbol *function;
    struct ExpressionList *arguments;
};

struct Expression {
    enum {
        Assignment,
        FunctionCall
    } type;
    union {
        struct Assignment assignment;
        struct FunctionCall functionCall;
    } value;
};

struct Expression functionCallExpression(struct Symbol *function, struct ExpressionList *arguments);
