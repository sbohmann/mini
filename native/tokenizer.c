#include "tokenizer.h"

#include <stdio.h>

#include "allocate.h"
#include "errors.h"

void retain(struct ComplexValue *instance) {

}

void release(struct ComplexValue *instance) {

}

static char * read_source(const char *path)
{
    FILE *file = fopen(path, "rb");
    non_null_errno(file, "Failed to open source path %s", path);
    non_negative_errno(fseek(file, 0, SEEK_END),
                       "fseek failed while reading source from path %s", path);
    long size = ftell(file);
    non_negative_errno(fseek(file, 0, SEEK_END),
                       "ftell failed while reading source from path %s", path);
    rewind(file);
    
    char *result = allocate_raw(size + 1);
    size_t bytes_read = fread(result, 1, size, file);
    if (bytes_read != size) {
        fail("fread read %ul bytes, expected: %ul, while reading source from path %s",
                bytes_read, size, path);
    }
    if (ferror(file) != 0) {
        fail("fread failed while reading source from path %s", path);
    }
    if (fclose(file) == EOF) {
        fail_errno("fread failed while reading source from path %s", path);
    }
    
    result[size] = 0;
    
    return result;
}

struct Tokens * read_file(const char *path) {
    struct Tokens *result = allocate(sizeof(struct Token));
    result->path = path;
    result->source = read_source(path);
    return result;
}
