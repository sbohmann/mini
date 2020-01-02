#include "struct.h"

#include <stdbool.h>
#include <core/allocate.h>
#include <stdlib.h>
#include <core/errors.h>

#include "core/typedefs.h"
#include "core/complex.h"

static const uint8_t MaximumLevel = 6;

struct ValueList {
    Name name;
    Value value;
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

void Struct_destructor(struct Struct *instance);

struct Struct *Struct_create() {
    struct Struct *result = allocate(sizeof(struct Struct));
    Complex_init(&result->base);
    result->base.destructor = (void (*) (struct ComplexValue *))Struct_destructor;
    result->base.type = StructComplexType;
    return result;
}

static void delete_values(struct ValueList *values) {
    if (values->next) {
        delete_values(values->next);
    }
    free(values);
}

static void delete_node(struct Node *node) {
    if (node->is_value_node) {
        delete_values(node->values);
    } else {
        for (size_t index = 0; index < 32; ++index) {
            if (node->sub_nodes[index]) {
                delete_node(node->sub_nodes[index]);
            }
        }
    }
    free(node);
}

void Struct_destructor(struct Struct *instance) {
    if (instance->root) {
        delete_node(instance->root);
    }
}

void Struct_release(struct Struct *instance) {
    release(&instance->base);
}

static size_t level_index(Hash hash, uint8_t level) {
    if (5 * level >= 32) {
        fail("Struct: illegal level: %d", (int) level);
    }
    return (hash >> (5 * level)) % 0x20;
}

static bool replace_value(struct ValueList *values, Name name, Value value) {
    while (values) {
        if (String_equal(values->name, name)) {
            Any_release(values->value);
            values->value = value;
            return true;
        }
        values = values->next;
    }
    return false;
}

static struct ValueList *insert_value(struct ValueList *existing_values, Name name, Value *value) {
    struct ValueList *new_values = allocate(sizeof(struct ValueList));
    new_values->name = name;
    new_values->value = (*value);
    new_values->next = existing_values;
    return new_values;
}

static struct Node *create_value_node(Name name, Hash hash, Value value) {
    struct Node *new_node = allocate(sizeof(struct Node));
    new_node->is_value_node = true;
    new_node->hash = hash;
    struct ValueList *values = allocate(sizeof(struct ValueList));
    values->name = name;
    values->value = value;
    new_node->values = values;
    return new_node;
}

static struct Node *Node_put(struct Node *node, uint8_t level, Name name, Hash hash, Value value, size_t *size) {
    size_t index = level_index(hash, level);
    if (node->is_value_node) {
        struct ValueList *existing_values = node->values;
        if (replace_value(existing_values, name, value)) {
            return node;
        } else if (level == MaximumLevel || node->hash == hash) {
//            printf("Collision!!!");
            node->values = insert_value(existing_values, name, &value);
            ++(*size);
            return node;
        } else {
            struct Node *new_node = allocate(sizeof(struct Node));
            size_t existing_node_index = level_index(node->hash, level);
            if (existing_node_index != index) {
                new_node->sub_nodes[existing_node_index] = node;
                new_node->sub_nodes[index] = create_value_node(name, hash, value);
                ++(*size);
                return new_node;
            } else {
                new_node->sub_nodes[existing_node_index] = Node_put(node, level + 1, name, hash, value, size);
                return new_node;
            }
        }
    } else {
        struct Node *existing_node = node->sub_nodes[index];
        if (existing_node) {
            node->sub_nodes[index] = Node_put(existing_node, level + 1, name, hash, value, size);
        } else {
            node->sub_nodes[index] = create_value_node(name, hash, value);
            ++(*size);
            return node;
        }
        return node;
    }
}

void Struct_put(struct Struct *self, Name name, Value value) {
    if (self->root) {
        self->root = Node_put(self->root, 0, name, name->hash, value, &self->size);
    } else {
        self->root = create_value_node(name, name->hash, value);
        self->size = 1;
    }
}

static bool Node_set(struct Node *node, uint8_t level, Name name, Hash hash, Value value) {
    if (node->is_value_node) {
        struct ValueList *values = node->values;
        while (values) {
            if (String_equal(values->name, name)) {
                values->value = value;
                return true;
            }
            values = values->next;
        }
        return false;
    } else {
        size_t index = level_index(hash, level);
        if (node->sub_nodes[index]) {
            return Node_set(node->sub_nodes[index], level + 1, name, hash, value);
        } else {
            return false;
        }
    }
}

bool Struct_set(struct Struct *self, Name name, Value value) {
    if (self->root) {
        return Node_set(self->root, 0, name, name->hash, value);
    } else {
        return false;
    }
}

static struct MapResult Node_get(struct Node *node, uint8_t level, Name name, Hash hash) {
    if (node->is_value_node) {
        struct ValueList *values = node->values;
        while (values) {
            if (String_equal(values->name, name)) {
                return (struct MapResult) { true, values->value };
            }
            values = values->next;
        }
        return (struct MapResult) { false, None() };
    } else {
        size_t index = level_index(hash, level);
        if (node->sub_nodes[index]) {
            return Node_get(node->sub_nodes[index], level + 1, name, hash);
        } else {
            return (struct MapResult) { false, None() };
        }
    }
}

struct MapResult Struct_get(struct Struct *self, Name name) {
    if (self->root) {
        return Node_get(self->root, 0, name, name->hash);
    } else {
        return (struct MapResult) { false, None() };
    }
}

static struct Node *Node_remove(struct Node *node, uint8_t level, Name name, Hash hash, bool *found) {
    if (node->is_value_node) {
        struct ValueList *values = node->values;
        struct ValueList **source = &node->values;
        while (values) {
            if (String_equal(values->name, name)) {
                if (*found) {
                    fail("Found multiple entries for name %zu", name);
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
            node->sub_nodes[index] = Node_remove(node->sub_nodes[index], level + 1, name, hash, found);
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

bool Struct_remove(struct Struct *self, Name name) {
    if (self->root) {
        bool found = false;
        self->root = Node_remove(self->root, 0, name, name->hash, &found);
        if (found) {
            --self->size;
        }
        return found;
    } else {
        return false;
    }
}
