#include "statement.h"

struct Statement functionDeclarationStatement(void) {
    return (struct Statement) {
            FunctionDeclaration,
            {
                    .functionDeclaration = {
                            1
                    }
            }
    };
}

struct Statement assignmentStatement(void) {
    return (struct Statement) {
            Assignment,
            {
                    .assignment = {
                    }
            }
    };
}
