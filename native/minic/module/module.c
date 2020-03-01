#include <core/allocate.h>
#include <minic/source.h>
#include <minic/tokens/token.h>
#include "module.h"

struct ParsedModule *ParsedModule_read(const char *path) {
    struct ParsedModule *result = allocate(sizeof(struct Token));
    result->path = path;
    result->source = read_source(path);
    result->tokens = read_tokens(path, result->source);
    result->elements = read_elements(result->tokens);
    return result;
}

void ParsedModule_delete(struct ParsedModule *instance) {
    // TODO create and run parser GC, leaving but pinned strings
}
