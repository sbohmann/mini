#include "source.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "allocate.h"
#include "errors.h"
#include "string.h"
#include "files.h"

struct StringList * prepend_line(struct StringList *lines, const char *pointer, const char *line_start) {
    size_t line_length = pointer - line_start;
    char *line_value = allocate(line_length + 1);
    memcpy(line_value, line_start, line_length);
    struct String *line = allocate(sizeof(struct String));
    *line = (struct String) {line_length, line_value};
    return StringList_prepend(lines, line);
}

struct StringList * collect_lines(const struct String *text, struct StringList *initial_lines) {
    struct StringList *lines = initial_lines;
    const char *iterator = text->value;
    const char *line_start = text->value;
    const char *end = text->value + text->length;
    int current_line_processed = 0;
    while (iterator != end) {
        if (*iterator == '\r' || *iterator == '\n') {
            if (!current_line_processed) {
                lines = prepend_line(lines, iterator, line_start);
                current_line_processed = 1;
            }
        }
        if (*iterator == '\n') {
            line_start = iterator + 1;
            current_line_processed = 0;
        }
        ++iterator;
    }
    if (line_start != end) {
        lines = prepend_line(lines, iterator, line_start);
    }
    return lines;
}

static struct Source *create_source(struct String *text) {
    struct StringList *lines = StringList_create();
    struct StringList *const last = lines;
    lines = collect_lines(text, lines);
    const size_t size = lines->size;
    
    struct String *result_lines = allocate(sizeof(struct String) * size);
    struct String *result_line_iterator = result_lines;
    for (struct StringList *current = last->previous; current != 0; current = current->previous) {
        if (result_line_iterator - result_lines >= size) {
            fail("Logical error in result lines creation");
        }
        *result_line_iterator = *current->value;
        ++result_line_iterator;
    }
    if (result_line_iterator - result_lines != size) {
        fail("Logical error in result lines creation - offset: [%zu], size: [%zu]",
             result_line_iterator - result_lines, size);
    }
    
    StringList_free(lines);
    
    struct Source *result = allocate(sizeof(struct Source));
    result->number_of_lines = size;
    result->lines = result_lines;
    return result;
}

struct Source *read_source(const char *path) {
    struct String *source_text = read_text_file(path);
    struct Source *result = create_source(source_text);
    free(source_text);
    return result;
}
