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
        Assignment,
        VariableDeclaration
    } type;
    union {
        struct FunctionDeclaration functionDeclaration;
        struct VariableDeclaration variableDeclaration;
        struct Assignment assignment;
    } value;
};

struct Statement functionDeclarationStatement(void);

struct Statement assignmentStatement(void);
