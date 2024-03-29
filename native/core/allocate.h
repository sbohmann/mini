#pragma once

#include <stdio.h>

void ParserGC_init(void);
void ParserGC_pause(void);
void ParserGC_resume(void);
void ParserGC_mark(const void *pointer);
void ParserGC_free(void);

void * allocate(size_t size);
void * allocate_unmanaged(size_t size);
void * allocate_raw(size_t size);
void * allocate_raw_unmanaged(size_t size);

void deallocate(void *pointer);
void deallocate_unmanaged(void *pointer);
