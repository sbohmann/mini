#include "expression.h"

struct FunctionCall {
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
