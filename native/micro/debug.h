#if DEBUG_ENABLED
#define DEBUG(format, ...) printf((format), __VA_ARGS__)
#else
#define DEBUG(format, ...)
#endif
