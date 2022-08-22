#pragma once

#include <stddef.h>

struct ASTString {
    const char *value;
    const size_t length;
};

struct ASTString ASTString_fromLiteral(const char *value);

struct ASTString ASTString_fromLiteralAndLength(const char *value, size_t length);
