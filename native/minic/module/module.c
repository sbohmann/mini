#include <core/allocate.h>
#include <minic/source.h>
#include <minic/tokens/token.h>
#include "module.h"

struct ParsedModule *read_file(const char *path) {
    struct ParsedModule *result = allocate(sizeof(struct Token));
    result->path = path;
    result->source = read_source(path);
    result->tokens = read_tokens(path, result->source);
    result->elements = read_elements(result->tokens);
    return result;
}
