#pragma once

#include <stdio.h>

static const size_t maximum_name_length = 127;

struct NameBuffer {
    char data[maximum_name_length + 1];
    size_t size;
};

extern struct NameBuffer name_buffer;

void print_name_buffer_content(FILE *file);
