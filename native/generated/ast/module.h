#pragma once

#include "ast/ast_types.h"
#include <stddef.h>

struct Module 
{
    const struct ASTString name;
    const size_t nameLength;
    const struct Statement * const statements;
    const size_t statementsLength;
};
