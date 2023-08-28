#pragma once

#include <stdbool.h>

struct FunctionDeclaration {
    bool constant;
};

struct VariableDeclaration {
    bool constant;
};

struct Assignment {
};

struct Statement {
    enum {
        FunctionDeclaration,
    } type;
    union {
        struct FunctionDeclaration functionDeclaration;
        struct VariableDeclaration variableDeclaration;
        struct Assignment assignment;
    } value;
};

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
                2
            }
        }
    };
}


struct Statement functionCallStatement(void) {
    return (struct Statement) {
        FunctionCall,
        {
            .functionCall = {
                3.0
            }
        }
    };
}
