#include <stdlib.h>

#include "string.h"
#include "allocate.h"
#include "errors.h"

struct StringList * StringList_create() {
    struct StringList * result = allocate(sizeof(struct StringList));
    *result = (struct StringList) { 0, 0, 0, 0 };
    return result;
}

struct StringList * StringList_prepend(struct StringList * list, const struct String * value) {
    if (list->previous) {
        fail("Attempting to prepend to a list with non-null previous field");
    }
    struct StringList * result = allocate(sizeof(struct StringList));
    *result = (struct StringList) { list->size + 1, value, list };
    result->next = list;
    list->previous = result;
    return result;
}

struct StringList * StringList_free(struct StringList * list) {
    if (list->next) {
        StringList_free(list->next);
    }
    free(list);
}
