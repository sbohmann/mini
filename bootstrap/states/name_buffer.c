#include "name_buffer.h"

struct NameBuffer name_buffer;

void print_name_buffer_content(FILE *file) {
    for (size_t index = 0; index < name_buffer.size; ++index) {
        putc(name_buffer.data[index], file);
    }
}
