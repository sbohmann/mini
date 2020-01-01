#include "print.h"

#include <stdio.h>
#include <string.h>
#include <collections/hashmap.h>
#include <core/complex.h>
#include <minic/list.h>
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
            if (value.complex_value->type == ListComplexType) {
                struct List *list = (struct List *)value.complex_value;
                putchar('[');
                for (size_t index = 0; index < list->size; ++index) {
                    if (index > 0) {
                        printf(", ");
                    }
                    print_value(List_get(list, index));
                }
                putchar(']');
            } else if (value.complex_value->type == StructComplexType) {
                printf("<struct>");
            } else {
                printf("<complex>");
            }
            break;
        case FlatType:
            printf("[");
            for (size_t index = 0; index < 8; ++index) {
                printf("%s%02x", (index > 0 ? " " : ""), (uint8_t) value.flat_value[index]);
            }
            printf("]");
            break;
        case FunctionPointerType:
            printf("<function>");
            break;
        default:
            printf("<unknown>");
    }
}
