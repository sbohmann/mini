#include "fs.h"

#include <core/files.h>
#include <core/errors.h>

struct Any micro_read_text_file(const struct List *arguments) {
    if (arguments->size != 1) {
        fail_with_message("Illegal number of arguments to read_text_file: %zu - expected 1", arguments->size);
    }
    struct Any path = List_get(arguments, 0);
    if (path.type != StringType) {
        fail_with_message("Illegal type of path [%s] - expected String", Any_typename(path));
    }
    // TODO return StringComplexType by using a function not designed for parsing
    return String(read_text_file(path.string->value));
}
