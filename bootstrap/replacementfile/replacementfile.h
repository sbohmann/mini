#pragma once

#include <stdio.h>

struct ReplacementFile;

struct ReplacementFile * replacementfile_alloc();

void replacementfile_init(struct ReplacementFile *self, FILE *source);

struct ReplacementFile * replacementfile_new(FILE *source);

const char * replacementfile_replace(struct ReplacementFile *self, const char *name);
