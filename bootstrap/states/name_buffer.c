const size_t name_buffer_length = 128;

struct NameBuffer name_buffer;

static void print_name_buffer_content(FILE *file) {
    for (size_t index = 0; index < name_state.size; ++index) {
        putc(name_buffer.data[index], stderr);
    }
}
