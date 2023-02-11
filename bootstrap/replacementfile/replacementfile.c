#include "replacementfile.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

struct ReplacementEntry {
    const char *name;
    const char *value;
    const struct ReplacementEntry *next;
};

struct ReplacementFile {
    FILE *source;
    struct ReplacementEntry *entries;
};

struct replacementfile * replacementfile_create(FILE *);

struct ReplacementFile * replacementfile_alloc() {
    struct ReplacementFile *result =
        (struct ReplacementFile *) calloc(1, sizeof(struct ReplacementFile));
    if (result == NULL) {
        fprintf(stderr, "replacementfile: Failed to allocate an instance"
            " of ReplacementFile.\n");
        abort();
    }
    return result;
}

void replacementfile_init(struct ReplacementFile *self, FILE *source) {
    self->source = source;
    self->entries = NULL;
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
