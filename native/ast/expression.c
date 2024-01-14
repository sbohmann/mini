#include "expression.h"

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

struct Expression functionCallExpression(struct Symbol *function, struct ExpressionList *arguments) {
    return (struct Expression) {
            FunctionCall,
            {
                    .functionCall = {
                            function,
                            arguments
                    }
            }
    };
}
