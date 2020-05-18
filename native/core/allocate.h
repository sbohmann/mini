#pragma once

#include <stdio.h>

void ParserGC_init();
void ParserGC_free();

void * allocate(size_t size);
void * allocate_raw(size_t size);
