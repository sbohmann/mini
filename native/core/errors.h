#pragma once

_Noreturn void fail(void);

_Noreturn void fail_with_message(const char *format, ...);

_Noreturn void fail_with_message_and_errno(const char *format, ...);

void non_null_errno(const void *pointer, const char *format, ...);

void non_negative_errno(long value, const char *format, ...);
