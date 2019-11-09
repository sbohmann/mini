#include "{{file}}.h"

#include <stdlib.h>

#include "core/allocate.h"
#include "core/errors.h"

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

struct {{name}}List * {{name}}List_delete(struct {{name}}List * instance) {
    struct {{name}}ListElement *element = instance->first;
    while (element) {
        struct {{name}}ListElement *next = element->next;
        free(element);
        element = next;
    }
    free(instance);
}

size_t {{name}}List_size(struct {{name}}List * self) {
    return self->size;
}

void {{name}}List_append(struct {{name}}List * self, {{prefix}}value) {
    struct {{name}}ListElement * element = allocate(sizeof(struct {{name}}ListElement));
    element->value = value;
    if (self->size == 0) {
        self->first = element;
        self->last = element;
        self->size = 1;
    } else {
        self->last->next = element;
        element->previous = self->last;
        self->last = element;
        ++self->size;
    }
}

void {{name}}List_prepend(struct {{name}}List * self, {{prefix}}value) {
    struct {{name}}ListElement * element = allocate(sizeof(struct {{name}}ListElement));
    element->value = value;
    if (self->size == 0) {
        self->first = element;
        self->last = element;
        self->size = 1;
    } else {
        self->first->previous = element;
        element->next = self->first;
        self->first = element;
        ++self->size;
    }
}

{{type}} * {{name}}List_to_array(struct {{name}}List * self) {
    const size_t size = {{name}}List_size(self);
    const size_t element_size = sizeof({{type}});
    const size_t result_size = element_size * size;
    {{type}} *result = allocate(result_size);
    {{type}} *result_iterator = result;
    struct {{name}}ListElement *iterator = {{name}}List_begin(self);
    while (iterator) {
        if (result_iterator - result > size - 1) {
            fail("Logical error in {{name}}List_to_array - target overrun");
        }
        *result_iterator = {{value_dereference}}{{name}}ListIterator_get(iterator);
        iterator = {{name}}ListIterator_next(iterator);
        ++result_iterator;
    }
    if (result_iterator - result != size) {
        fail("Logical error in result lines creation - offset: [%zu], size: [%zu]",
             result_iterator - result, size);
    }
    return result;
}

struct {{name}}ListElement * {{name}}List_begin(struct {{name}}List * self) {
    return self->first;
}

struct {{name}}ListElement * {{name}}List_end(struct {{name}}List * self) {
    return self->last;
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

