#include "tokenizer.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "allocate.h"
#include "errors.h"

void retain(struct ComplexValue *instance) {

}

void release(struct ComplexValue *instance) {

}

void append(struct StringList *list) {

}

static struct String *read_source_text(const char *path) {
    FILE *file = fopen(path, "rb");
    non_null_errno(file, "Failed to open source path %s", path);
    non_negative_errno(fseek(file, 0, SEEK_END),
                       "fseek failed while reading source from path %s", path);
    long size = ftell(file);
    non_negative_errno(fseek(file, 0, SEEK_END),
                       "ftell failed while reading source from path %s", path);
    rewind(file);
    
    char *raw_result = allocate_raw(size + 1);
    size_t bytes_read = fread(raw_result, 1, size, file);
    if (bytes_read != size) {
        fail("fread read %ul bytes, expected: %ul, while reading source from path %s",
             bytes_read, size, path);
    }
    if (fclose(file) == EOF) {
        fail_errno("fread failed while reading source from path %s", path);
    }
    
    raw_result[size] = 0;
    
    struct String *result = allocate(sizeof(struct String));
    *result = (struct String) {size, raw_result};
    return result;
}

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
    while (iterator != end) {
        if (*iterator == '\r' || *iterator == '\n') {
            lines = prepend_line(lines, iterator, line_start);
        }
        if (*iterator == '\n') {
            line_start = iterator + 1;
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
    
    struct String *result_lines = allocate(sizeof(struct String) * lines->size);
    struct String *result_line_iterator = result_lines;
    for (struct StringList *current = last->previous; current != 0; current = current->previous) {
        if (result_line_iterator - result_lines >= lines->size) {
            fail("Logical error in result lines creation");
        }
        *result_line_iterator = *current->value;
        ++result_line_iterator;
    }
    if (result_line_iterator - result_lines != lines->size) {
        fail("Logical error in result lines creation - offset: [%zu], size: [%zu]",
                result_line_iterator - result_lines, lines->size);
    }
    
    StringList_free(lines);
    
    struct Source *result = allocate(sizeof(struct Source));
    result->number_of_lines = lines->size;
    result->lines = result_lines;
    return result;
}

static struct Source *read_source(const char *path) {
    struct String *source_text = read_source_text(path);
    struct Source *result = create_source(source_text);
    free(source_text);
    return result;
}

struct Tokens *read_file(const char *path) {
    struct Tokens *result = allocate(sizeof(struct Token));
    result->path = path;
    result->source = read_source(path);
    return result;
}
