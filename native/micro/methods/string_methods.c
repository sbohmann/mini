#include "string_methods.h"

#include <memory.h>

#include <core/errors.h>
#include <core/allocate.h>
#include <core/string.h>
#include <minic/list.h>
#include <core/stringbuilder.h>

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
        List_add(list, StringLiteral(String_preallocated(fragment, length)));
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

static bool is_whitespace(char c) {
    switch (c) {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            return true;
        default:
            return false;
    }
}

struct Any String_trim(const struct String *value) {
    struct StringBuilder *buffer = StringBuilder_create();
    size_t start = 0;
    bool collecting = false;
    for (size_t index = 0; index < value->length; ++index) {
        char c = value->value[index];
        if (!is_whitespace(c)) {
            if (collecting) {
                for (size_t whitespace_index = start; whitespace_index < index; ++whitespace_index) {
                    StringBuilder_append(buffer, value->value[whitespace_index]);
                }
            } else {
                collecting = true;
            }
            StringBuilder_append(buffer, c);
            start = index + 1;
        }
    }
    // TODO use complex string
    // creating new string literals on the fly will leak memory
    return StringLiteral(StringBuilder_result(buffer));
}
