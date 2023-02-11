#pragma once

#include <stdio.h>

#define MINI_BOOTSTRAP_NAMEBUF_MAXIMUM_NAME_LENGTH 127

static const size_t maximum_name_length = MINI_BOOTSTRAP_NAMEBUF_MAXIMUM_NAME_LENGTH;

struct NameBuffer {
    char data[MINI_BOOTSTRAP_NAMEBUF_MAXIMUM_NAME_LENGTH + 1];
    size_t size;
};

extern struct NameBuffer name_buffer;

void print_name_buffer_content(FILE *file);
