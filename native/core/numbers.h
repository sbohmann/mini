#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

int64_t parse_integer(const char *str, size_t len, uint8_t radix, bool isSigned);
