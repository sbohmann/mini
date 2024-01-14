#include "replacementfile.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

struct ReplacementEntry {
    const char *name;
    const char *value;
    struct ReplacementEntry *next;
};

struct ReplacementFile {
    FILE *source;
    struct ReplacementEntry *entries;
    struct ReplacementEntry **nextEntry;
};

enum ReadingState {
    Name,
    Value
};

struct replacementfile * replacementfile_create(FILE *);

static void read_replacements(struct ReplacementFile *self);

static const char *string(const char *source, size_t length);

static void add_entry(struct ReplacementFile *self, const char *name, const char *value);

struct ReplacementFile * replacementfile_alloc() {
    struct ReplacementFile *result =
        (struct ReplacementFile *) calloc(1, sizeof(struct ReplacementFile));
    if (result == NULL) {
        fprintf(stderr, "replacementfile: Failed to allocate an instance"
            " of ReplacementFile.\n");
        exit(1);
    }
    return result;
}

void replacementfile_init(struct ReplacementFile *self, FILE *source) {
    self->source = source;
    self->entries = NULL;
    self->nextEntry = &self->entries;
    read_replacements(self);
}

struct ReplacementFile * replacementfile_new(FILE *source) {
    struct ReplacementFile *self = replacementfile_alloc();
    replacementfile_init(self, source);
    return self;
}

static void read_replacements(struct ReplacementFile *self) {
    char name_buffer[128];
    char value_buffer[1024];
    size_t name_length = 0;
    size_t value_length = 0;
    size_t line = 1;
    enum ReadingState state = Name;
    bool end_of_file = false;
    while (!end_of_file) {
        int c = fgetc(self->source);
        bool end_of_line = false;
        bool new_line = false;
        if (c < 0) {
            end_of_line = true;
            end_of_file = true;
        } else if (c == '=') {
            if (state != Name || name_length == 0) {
                fprintf(stderr, "Unexpected = character at line %zu\n", line);
                exit(1);
            }
            state = Value;
        } else if (c == '\n') {
            end_of_line = true;
            new_line = true;
        } else if (c == '\r') {
            continue;
        } else {
            if (state == Name) {
                if (name_length >= 127) {
                    fprintf(stderr, "Name length exceeded at line %zu\n", line);
                    exit(1);
                }
                name_buffer[name_length] = c;
                ++name_length;
            } else if (state == Value) {
                if (value_length >= 1023) {
                    fprintf(stderr, "Value length exceeded at line %zu\n", line);
                    exit(1);
                }
                value_buffer[value_length] = c;
                ++value_length;
            } else {
                fprintf(stderr, "Logical error at line %zu\n", line);
                exit(1);
            }
        }
        
        if (end_of_line) {
            if (state != Name || name_length != 0) {
                if (state != Value || value_length == 0) {
                    fprintf(stderr, "Unexpected end of line %zu\n", line);
                    exit(1);
                }
                const char *name = string(name_buffer, name_length);
                const char *value = string(value_buffer, value_length);
                add_entry(self, name, value);
                name_length = 0;
                value_length = 0;
                state = Name;
            }
        }
        
        if (new_line) {
            ++line;
        }
    }
}

static const char *string(const char *source, size_t length) {
    char *result = malloc(length + 1);
    if (result == NULL) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
    memcpy(result, source, length);
    result[length] = 0;
    return result;
}

static void add_entry(struct ReplacementFile *self, const char *name, const char *value) {
    struct ReplacementEntry *new_entry =
        (struct ReplacementEntry *) malloc(sizeof(struct ReplacementEntry));
    if (new_entry == NULL) {
        fprintf(stderr, "Out of memory");
        exit(1);
    }
    new_entry->name = name;
    new_entry->value = value;
    new_entry->next = NULL;
    *(self->nextEntry) = new_entry;
    self->nextEntry = &new_entry->next;
}

const char * replacementfile_replace(struct ReplacementFile *self, const char *name) {
    const struct ReplacementEntry *entry = self->entries;
    while (entry != NULL) {
        if (strcmp(name, entry->name) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    return NULL;
}
