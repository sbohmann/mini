#include "print.h"

#include <stdio.h>
#include <string.h>
#include <collections/hashmap.h>
#include "debug.h"

void print_value(struct Any value) {
    switch (value.type) {
        case NoneType:
            printf("None");
            break;
        case IntegerType:
            printf("%d", (int) value.integer);
            break;
        case StringType:
            printf("%s", value.string->value);
            break;
        case ComplexType:
            // TODO
            printf("<complex>");
            break;
        case FlatType:
            printf("[");
            for (size_t index = 0; index < 8; ++index) {
                printf("%s%02x", (index > 0 ? " " : ""), (uint8_t) value.flat_value[index]);
            }
            printf("]");
            break;
        default:
            printf("<unknown>");
    }
}
