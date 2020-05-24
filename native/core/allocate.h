#pragma once

#include <stdio.h>

void ParserGC_init(void);
void ParserGC_pause();
void ParserGC_resume();
void ParserGC_mark(void *pointer);
void ParserGC_free(void);

void * allocate(size_t size);
void * allocate_unmanaged(size_t size);
void * allocate_raw(size_t size);
void * allocate_raw_unmanaged(size_t size);
void deallocate(void *pointer);
