#pragma once

#include <stdbool.h>

extern const char *start_signature;
extern const size_t start_signature_length;

extern const char *end_signature;
extern const size_t end_signature_length;

bool is_name_start(char c);
bool is_name_part(char c);
