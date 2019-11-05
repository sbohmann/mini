#include "{{name}}List.h"

#include "allocate.h"

struct {{name}}List {
    size_t size;
    struct ListElement *first;
    struct ListElement *last;
};

struct {{name}}ListElement {
    {{prefix}}value;
    {{name}}ListElement *next;
    {{name}}ListElement *previous;
};

{{name}}List * {{name}}List_create() {
    return allocate(sizeof(struct {{name}}List));
}

{{name}}List * {{name}}List_delete(struct {{name}}List * list) {
    {{name}}ListElement *element = list->first;
    while (element) {
        {{name}}ListElement *next = element->next;
        free(element);
        element = next;
    }
}

void {{name}}List_append(struct {{name}}List * list, {{prefix}}value) {
    {{name}}ListElement * element = allocate(sizeof({{name}}ListElement));
    element->value = value;
    if (list->size == 0) {
        list->first = element
        list->last = element
        list->size = 1;
    } else {
        list->last->next = element;
        element->previous = list->last;
        list->last = element;
        ++list->size;
    }
}

void {{name}}List_prepend(struct {{name}}List * list, {{prefix}}value) {
    {{name}}ListElement * element = allocate(sizeof({{name}}ListElement));
    element->value = value;
    if (list->size == 0) {
        list->first = element
        list->last = element
        list->size = 1;
    } else {
        list->first->previous = element;
        element->next = list->first;
        list->first = element;
        ++list->size;
    }
}

{{name}}ListElement * {{name}}List_begin(struct {{name}}List * list) {
    return list->first;
}

{{name}}ListElement * {{name}}List_end(struct {{name}}List * list) {
    return list->last;
}

{{name}}ListElement * {{name}}ListIterator_next({{name}}ListElement * iterator) {
    if (iterator) {
        return iterator->next;
    } else {
        fail("Attempt to get next from null iterator");
    }
}

{{name}}ListElement * {{name}}ListIterator_previous({{name}}ListElement * iterator) {
    if (iterator) {
        return iterator->previous;
    } else {
        fail("Attempt to get previous from null iterator");
    }
}

{{value}} {{name}}ListIterator_get({{name}}ListElement * value) {
    if (iterator) {
        return iterator->value;
    } else {
        fail("Attempt to get value from null iterator");
    }
}

