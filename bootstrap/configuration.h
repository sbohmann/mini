#include <string.h>

static const char *start_signature = "@_";
static const size_t start_signature_length = sizeof(start_signature) - 1;

static const char *end_signature = ";";
static const size_t end__signature_length = sizeof(end_signature) - 1;

static bool is_name_part(c) {
    return c == '_' ||
        (c >= 'A' && c <= 'Z') ||
        (c >= 'a' && c <= 'z') ||
        (c >= '0' && c <= '9');
}
