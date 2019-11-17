#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

int64_t parse_int64(const char *str, size_t len, uint8_t radix);
int64_t parse_uint64(const char *str, size_t len, uint8_t radix);
