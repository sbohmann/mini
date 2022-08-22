#include <string.h>
#include "ast_types.h"

struct ASTString ASTString_fromLiteral(const char *value) {
    return (struct ASTString) {
            value,
            strlen(value)
    };
}

struct ASTString ASTString_fromLiteralAndLength(const char *value, size_t length) {
    return (struct ASTString) {
        value,
        length
    };
}
