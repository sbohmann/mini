#include <stddef.h>

#include "configuration.h"

const char *start_signature = "@_";
const size_t start_signature_length = sizeof(start_signature) - 1;

const char *end_signature = ";";
const size_t end_signature_length = sizeof(end_signature) - 1;

bool is_name_part(char c) {
    return c == '_' ||
        (c >= 'A' && c <= 'Z') ||
        (c >= 'a' && c <= 'z') ||
        (c >= '0' && c <= '9');
}
