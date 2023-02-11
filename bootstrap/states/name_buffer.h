#pragma once

#include <stdio.h>

static const size_t name_buffer_length = 128;

struct NameBuffer {
    char data[name_buffer_length];
    size_t size;
};

extern struct NameBuffer name_buffer;

void print_name_buffer_content(FILE *file);
