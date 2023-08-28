#include "expression.h"

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
        struct FunctionCall functionCall;
    } value;
};

struct Expression functionCallSExpression(void) {
    return (struct Expression) {
            FunctionCall,
            {
                    .functionCall = {
                    }
            }
    };
}
