#pragma once

struct FunctionDeclaration {
    int a;
};

struct Assignment {
    int b;
};

struct FunctionCall {
    double c;
};

struct Statement {
    enum {
        FunctionDeclaration,
        Assignment,
        FunctionCall
    } type;
    union {
        struct FunctionDeclaration functionDeclaration;
        struct Assignment assignment;
        struct FunctionCall functionCall;
    } value;
};

struct Statement functionDeclarationStatement() {
    return (struct Statement) {
        FunctionDeclaration,
        {
            .functionDeclaration = {
                1
            }
        }
    };
}

struct Statement assignmentStatement() {
    return (struct Statement) {
        Assignment,
        {
            .assignment = {
                2
            }
        }
    };
}


struct Statement functionCallStatement() {
    return (struct Statement) {
        FunctionCall,
        {
            .functionCall = {
                3.0
            }
        }
    };
}
