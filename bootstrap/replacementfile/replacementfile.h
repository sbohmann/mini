#pragma once

#include <stdio.h>

struct ReplacementFile;

struct ReplacementFile * replacementfile_alloc();

void replacementfile_init(struct ReplacementFile *self, FILE *source);

static struct ReplacementFile * replacementfile_new(FILE *source) {
    struct ReplacementFile *self = replacementfile_alloc(); 
    replacementfile_init(self, source);
    return self;
}

const char * replacementfile_replace(struct ReplacementFile *self, const char *name);
