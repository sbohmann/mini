#include "hashmap.h"

#include <stdbool.h>
#include <core/allocate.h>
#include <stdlib.h>
#include <core/errors.h>

#include "core/complex.h"

static const uint8_t MaximumLevel = 12;

struct ValueList {
    Key key;
    Value value;
    struct ValueList *next;
};

struct Node {
    bool is_value_node;
    union {
        struct Node *sub_nodes[32];
        struct ValueList *values;
    };
};

struct HashMap {
    size_t size;
    struct Node *root;
};

struct HashMap *HashMap_create() {
    return allocate(sizeof(struct HashMap));
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
}

void HashMap_delete(struct HashMap *instance) {
    if (instance->root) {
        delete_node(instance->root);
    }
    free(instance);
}

static size_t level_index(Key key, uint8_t level) {
    if (5 * level >= 64) {
        fail("HashMap: illegal level: %d", (int) level);
    }
    return (key >> (64u - 5 * level)) % 0x20;
}

bool replace_value(struct ValueList *values, Key key, Value value) {
    while (values) {
        if (values->key == key) {
            Any_release(values->value);
            values->value = value;
            return true;
        }
        values = values->next;
    }
    return false;
}

struct ValueList *insert_value(struct ValueList *existing_values, Key key, Value *value) {
    struct ValueList *new_values = allocate(sizeof(struct ValueList));
    new_values->key = key;
    new_values->value = (*value);
    new_values->next = existing_values;
    return new_values;
}

struct Node *create_value_node(Key key, Value value) {
    struct Node *new_node = allocate(sizeof(struct Node));
    struct ValueList *values = allocate(sizeof(struct ValueList));
    values->key = key;
    values->value = value;
    new_node->is_value_node = true;
    new_node->values = values;
    return new_node;
}

static struct Node *Node_put(struct Node *node, uint8_t level, Key key, Value value, size_t *size) {
    size_t index = level_index(key, level);
    if (node->is_value_node) {
        struct ValueList *existing_values = node->values;
        if (replace_value(existing_values, key, value)) {
            return node;
        } else if (level == MaximumLevel) {
            node->values = insert_value(existing_values, key, &value);
            ++(*size);
            return node;
        } else {
            struct Node *new_node = allocate(sizeof(struct Node));
            new_node->sub_nodes[index] = node;
            ++(*size);
            return new_node;
        }
    } else {
        struct Node *existing_node = node->sub_nodes[index];
        if (existing_node) {
            node->sub_nodes[index] = Node_put(existing_node, level + 1, key, value, size);
        } else {
            node->sub_nodes[index] = create_value_node(key, value);
            ++(*size);
            return node;
        }
        return node;
    }
}

void HashMap_put(struct HashMap *self, Key key, struct Any value) {
    if (self->root) {
        self->root = Node_put(self->root, 0, key, value, &self->size);
    }
}

Value Node_get(struct Node *node, uint8_t level, Key key) {
    if (node->is_value_node) {
        struct ValueList *values = node->values;
        while (values) {
            if (values->key == key) {
                return values->value;
            }
        }
        return Undefined();
    } else {
        size_t index = level_index(key, level);
        if (node->sub_nodes[index]) {
            return Node_get(node->sub_nodes[index], level + 1, key);
        } else {
            return Undefined();
        }
    }
}

Value HasMap_get(struct HashMap *self, Key key) {
    if (self->root) {
        return Node_get(self->root, 0, key);
    } else {
        return Undefined();
    }
}

struct Node *Node_remove(struct Node *node, uint8_t level, Key key, bool *found) {
    if (node->is_value_node) {
        struct ValueList *values = node->values;
        struct ValueList **source = &node->values;
        while (values) {
            if (values->key == key) {
                *source = values->next;
                free(values);
                *found = true;
            }
            source = &values->next;
        }
        if (node->values) {
            return node;
        } else {
            free(node);
            return 0;
        }
    } else {
        size_t index = level_index(key, level);
        if (node->sub_nodes[index]) {
            node->sub_nodes[index] = Node_remove(node->sub_nodes[index], level + 1, key, found);
            for (size_t index = 0; index < 32; ++index) {
                if (node->sub_nodes[index]) {
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

bool HashMap_remove(struct HashMap *self, Key key) {
    if (self->root) {
        bool found = false;
        self->root = Node_remove(self->root, 0, key, &found);
        if (found) {
            --self->size;
        }
        return found;
    } else {
        return false;
    }
}
