#include <stddef.h>

struct Module
{
    const char * const name;
    const size_t nameLength;
    const struct Statement * const statements;
    const size_t statementsLength;
};
