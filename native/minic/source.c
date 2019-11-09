#include "source.h"

#include <stdlib.h>
#include <memory.h>

#include "core/allocate.h"
#include "core/errors.h"
#include "core/string.h"
#include "core/files.h"
#include "generated/string_list.h"

static void append_line(struct StringList *lines, const char *pointer, const char *line_start) {
    size_t line_length = pointer - line_start;
    char *line_value = allocate(line_length + 1);
    memcpy(line_value, line_start, line_length);
    struct String *line = allocate(sizeof(struct String));
    *line = (struct String) {line_length, line_value};
    StringList_append(lines, line);
}

static void collect_lines(const struct String *text, struct StringList *lines) {
    const char *iterator = text->value;
    const char *line_start = text->value;
    const char *end = text->value + text->length;
    int current_line_processed = 0;
    while (iterator != end) {
        if (*iterator == '\r' || *iterator == '\n') {
            if (!current_line_processed) {
                append_line(lines, iterator, line_start);
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
        append_line(lines, iterator, line_start);
    }
}

static struct String *flatten(struct StringList *lines) {
    struct String *result_lines = StringList_to_array(lines);
    StringList_delete(lines);
    return result_lines;
}

static struct Source *create_source(struct String *text) {
    struct StringList *lines = StringList_create();
    collect_lines(text, lines);
    const size_t size = StringList_size(lines);
    
    struct String *result_lines = flatten(lines);
    
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
