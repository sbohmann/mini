#pragma once

_Noreturn void fail(void);

_Noreturn void fail_with_message(const char *format, ...);

// TODO change to require an explicit errno argument from immediate capture - arguments may be provided via function calls providing the errno!
_Noreturn void fail_with_message_and_errno(const char *format, ...);

void non_null_errno(const void *pointer, const char *format, ...);

void non_negative_errno(long value, const char *format, ...);
