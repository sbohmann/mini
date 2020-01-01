#include "hashset.h"

#include <stdbool.h>
#include <core/allocate.h>
#include <stdlib.h>
#include <core/errors.h>

#include "core/typedefs.h"
#include "core/complex.h"

static const uint8_t MaximumLevel = 6;

struct ValueList {
    Element element;
    struct ValueList *next;
};

struct Node {
    bool is_value_node;
    union {
        struct Node *sub_nodes[32];
        struct {
            Hash hash;
            struct ValueList *values;
        };
    };
};

struct HashSet {
    struct ComplexValue base;
    size_t size;
    struct Node *root;
};

void HashSet_destructor(struct HashSet *instance);

struct HashSet *HashSet_create() {
    struct HashSet *result = allocate(sizeof(struct HashSet));
    Complex_init(&result->base);
    result->base.destructor = (void (*) (struct ComplexValue *))HashSet_destructor;
    result->base.type = SetComplexType;
    return result;
}

static void delete_node(struct Node *node) {
    if (!node->is_value_node) {
        for (size_t index = 0; index < 32; ++index) {
            if (node->sub_nodes[index]) {
                delete_node(node->sub_nodes[index]);
            }
        }
    }
    free(node);
}

void HashSet_destructor(struct HashSet *instance) {
    if (instance->root) {
        delete_node(instance->root);
    }
}

void HashSet_release(struct HashSet *instance) {
    release(&instance->base);
}

static size_t level_index(Hash hash, uint8_t level) {
    if (5 * level >= 32) {
        fail("HashSet: illegal level: %d", (int) level);
    }
    return (hash >> (5 * level)) % 0x20;
}

static bool find_element(struct ValueList *values, Element element) {
    while (values) {
        if (Any_equal(values->element, element)) {
            return true;
        }
        values = values->next;
    }
    return false;
}

static struct ValueList *insert_element(struct ValueList *existing_values, Element element) {
    struct ValueList *new_values = allocate(sizeof(struct ValueList));
    new_values->element = element;
    new_values->next = existing_values;
    return new_values;
}

static struct Node *create_value_node(Element element, Hash hash) {
    struct Node *new_node = allocate(sizeof(struct Node));
    new_node->is_value_node = true;
    new_node->hash = hash;
    struct ValueList *values = allocate(sizeof(struct ValueList));
    values->element = element;
    new_node->values = values;
    return new_node;
}

static struct Node *Node_add(struct Node *node, uint8_t level, Element element, Hash hash, size_t *size) {
    size_t index = level_index(hash, level);
    if (node->is_value_node) {
        struct ValueList *existing_values = node->values;
        if (find_element(existing_values, element)) {
            return node;
        } else if (level == MaximumLevel || node->hash == hash) {
//            printf("Collision!!!");
            node->values = insert_element(existing_values, element);
            ++(*size);
            return node;
        } else {
            struct Node *new_node = allocate(sizeof(struct Node));
            size_t existing_node_index = level_index(node->hash, level);
            if (existing_node_index != index) {
                new_node->sub_nodes[existing_node_index] = node;
                new_node->sub_nodes[index] = create_value_node(element, hash);
                ++(*size);
                return new_node;
            } else {
                new_node->sub_nodes[existing_node_index] = Node_add(node, level + 1, element, hash, size);
                return new_node;
            }
        }
    } else {
        struct Node *existing_node = node->sub_nodes[index];
        if (existing_node) {
            node->sub_nodes[index] = Node_add(existing_node, level + 1, element, hash, size);
        } else {
            node->sub_nodes[index] = create_value_node(element, hash);
            ++(*size);
            return node;
        }
        return node;
    }
}

void HashSet_add(struct HashSet *self, Element element) {
    if (self->root) {
        self->root = Node_add(self->root, 0, element, Any_hash(element), &self->size);
    } else {
        self->root = create_value_node(element, Any_hash(element));
        self->size = 1;
    }
}

static bool Node_contains(struct Node *node, uint8_t level, Element element, Hash hash) {
    if (node->is_value_node) {
        struct ValueList *values = node->values;
        while (values) {
            if (Any_equal(values->element, element)) {
                return true;
            }
            values = values->next;
        }
        return false;
    } else {
        size_t index = level_index(hash, level);
        if (node->sub_nodes[index]) {
            return Node_contains(node->sub_nodes[index], level + 1, element, hash);
        } else {
            return false;
        }
    }
}

bool HashSet_contains(struct HashSet *self, Element element) {
    if (self->root) {
        return Node_contains(self->root, 0, element, Any_hash(element));
    } else {
        return false;
    }
}

static struct Node *Node_remove(struct Node *node, uint8_t level, Element element, Hash hash, bool *found) {
    if (node->is_value_node) {
        struct ValueList *values = node->values;
        struct ValueList **source = &node->values;
        while (values) {
            if (Any_equal(values->element, element)) {
                if (*found) {
                    fail("Found multiple entries for element %zu", element);
                }
                struct ValueList *next = values->next;
                free(values);
                *source = next;
                values = next;
                *found = true;
            } else {
                source = &values->next;
                values = values->next;
            }
        }
        if (node->values) {
            return node;
        } else {
            free(node);
            return 0;
        }
    } else {
        size_t index = level_index(hash, level);
        if (node->sub_nodes[index]) {
            node->sub_nodes[index] = Node_remove(node->sub_nodes[index], level + 1, element, hash, found);
            for (size_t search_index = 0; search_index < 32; ++search_index) {
                if (node->sub_nodes[search_index]) {
                    return node;
                }
            }
            free(node);
            return 0;
        } else {
            return node;
        }
    }
}

bool HashSet_remove(struct HashSet *self, Element element) {
    if (self->root) {
        bool found = false;
        self->root = Node_remove(self->root, 0, element, Any_hash(element), &found);
        if (found) {
            --self->size;
        }
        return found;
    } else {
        return false;
    }
}
