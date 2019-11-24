#define DEBUG_ENABLED true

#if DEBUG_ENABLED
#include <stdio.h>
#define DEBUG(format, ...) printf((format), __VA_ARGS__)
#else
#define DEBUG(format, ...)
#endif
