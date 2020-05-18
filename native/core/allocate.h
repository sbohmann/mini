#pragma once

#include <stdio.h>

void ParserGC_init(void);
void ParserGC_free(void);

void * allocate(size_t size);
void * allocate_raw(size_t size);
