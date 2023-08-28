#pragma once

#include "ast/ast_types.h"
#include <stddef.h>

struct Module 
{
    const struct ASTString name;
    const struct Statement * const statements;
    const size_t statementsLength;
};
