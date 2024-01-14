#include "expression.h"

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
