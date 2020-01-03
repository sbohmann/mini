#include "string_split.h"

#include <memory.h>

#include <minic/list.h>
#include <core/errors.h>
#include <core/allocate.h>

static bool matches(const struct String *value, const struct String *separator, size_t index) {
    for (size_t offset = 0; offset < separator->length; ++offset) {
        if (value->value[index + offset] != separator->value[offset]) {
            return false;
        }
    }
    return true;
}

void add_fragment(struct List *list, const struct String *input, size_t start, size_t end) {
    size_t length = end - start;
    if (end > start) {
        char *fragment = allocate_raw(length + 1);
        memcpy(fragment, input->value + start, length);
        fragment[length] = 0;
        List_add(list, String(String_preallocated(fragment, length)));
    }
}

// TODO switch to a more refined algorithm after writing tests
struct Any String_split(const struct String *value, const struct String *separator) {
    struct List *result = List_create();
    if (separator->length == 0) {
        fail("String.split: separator length is zero");
    }
    size_t current_start = 0;
    for (size_t index = 0; index <= value->length - separator->length;) {
        if (matches(value, separator, index)) {
            add_fragment(result, value, current_start, index);
            index += separator->length;
            current_start = index;
        } else {
            ++index;
        }
    }
    add_fragment(result, value, current_start, value->length);
    return Complex(&result->base);
}
