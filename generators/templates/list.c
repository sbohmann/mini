#include "{{file}}.h"

#include <stdlib.h>

#include "../allocate.h"
#include "../errors.h"

struct {{name}}List {
    size_t size;
    struct {{name}}ListElement *first;
    struct {{name}}ListElement *last;
};

struct {{name}}ListElement {
    {{prefix}}value;
    struct {{name}}ListElement *next;
    struct {{name}}ListElement *previous;
};

struct {{name}}List * {{name}}List_create() {
    return allocate(sizeof(struct {{name}}List));
}

struct {{name}}List * {{name}}List_delete(struct {{name}}List * list) {
    struct {{name}}ListElement *element = list->first;
    while (element) {
        struct {{name}}ListElement *next = element->next;
        free(element);
        element = next;
    }
}

void {{name}}List_append(struct {{name}}List * list, {{prefix}}value) {
    struct {{name}}ListElement * element = allocate(sizeof(struct {{name}}ListElement));
    element->value = value;
    if (list->size == 0) {
        list->first = element;
        list->last = element;
        list->size = 1;
    } else {
        list->last->next = element;
        element->previous = list->last;
        list->last = element;
        ++list->size;
    }
}

void {{name}}List_prepend(struct {{name}}List * list, {{prefix}}value) {
    struct {{name}}ListElement * element = allocate(sizeof(struct {{name}}ListElement));
    element->value = value;
    if (list->size == 0) {
        list->first = element;
        list->last = element;
        list->size = 1;
    } else {
        list->first->previous = element;
        element->next = list->first;
        list->first = element;
        ++list->size;
    }
}

struct {{name}}ListElement * {{name}}List_begin(struct {{name}}List * list) {
    return list->first;
}

struct {{name}}ListElement * {{name}}List_end(struct {{name}}List * list) {
    return list->last;
}

struct {{name}}ListElement * {{name}}ListIterator_next(struct {{name}}ListElement * iterator) {
    if (iterator) {
        return iterator->next;
    } else {
        fail("Attempt to get next from null iterator");
    }
}

struct {{name}}ListElement * {{name}}ListIterator_previous(struct {{name}}ListElement * iterator) {
    if (iterator) {
        return iterator->previous;
    } else {
        fail("Attempt to get previous from null iterator");
    }
}

{{value}} {{name}}ListIterator_get(struct {{name}}ListElement * iterator) {
    if (iterator) {
        return iterator->value;
    } else {
        fail("Attempt to get value from null iterator");
    }
}

