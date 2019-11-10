#include <stdio.h>

#include "allocate.h"
#include "errors.h"
#include "string.h"

struct String *read_text_file(const char *path) {
    FILE *file = fopen(path, "rb");
    non_null_errno(file, "Failed to open source path %s", path);
    non_negative_errno(fseek(file, 0, SEEK_END),
                       "fseek failed while reading source from path %s", path);
    long size = ftell(file);
    non_negative_errno(fseek(file, 0, SEEK_END),
                       "ftell failed while reading source from path %s", path);
    rewind(file);
    
    char *raw_result = allocate_raw((size_t) size + 1);
    size_t bytes_read = fread(raw_result, 1, (size_t) size, file);
    if (bytes_read != size) {
        fail("fread read %l bytes, expected: %l, while reading source from path %s",
             bytes_read, size, path);
    }
    if (fclose(file) == EOF) {
        fail_errno("fread failed while reading source from path %s", path);
    }
    
    raw_result[size] = 0;
    
    struct String *result = allocate(sizeof(struct String));
    *result = (struct String) {size, raw_result};
    return result;
}
