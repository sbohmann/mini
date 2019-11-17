#include "hashmap.h"

#include <stdbool.h>
#include <core/allocate.h>
#include <stdlib.h>
#include <core/errors.h>

#include "core/complex.h"

static const uint8_t MaximumLevel = sizeof(size_t) == 8 ? 12 : 6;

struct ValueList {
    Key key;
    Value value;
    struct ValueList *next;
};

struct Node {
    bool is_value_node;
    union {
        struct Node *sub_nodes[32];
        struct {
            size_t hash;
            struct ValueList *values;
        };
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

size_t calculate_hash(Key key) {
    // TODO
    return key;
}

static size_t level_index(size_t hash, uint8_t level) {
    if (5 * level >= 64) {
        fail("HashMap: illegal level: %d", (int) level);
    }
    return (hash >> (64u - 5 * level)) % 0x20;
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

struct Node *create_value_node(Key key, size_t hash, Value value) {
    struct Node *new_node = allocate(sizeof(struct Node));
    new_node->is_value_node = true;
    new_node->hash = hash;
    struct ValueList *values = allocate(sizeof(struct ValueList));
    values->key = key;
    values->value = value;
    new_node->values = values;
    return new_node;
}

static struct Node *Node_put(struct Node *node, uint8_t level, Key key, size_t hash, Value value, size_t *size) {
    size_t index = level_index(hash, level);
    if (node->is_value_node) {
        struct ValueList *existing_values = node->values;
        if (replace_value(existing_values, key, value)) {
            return node;
        } else if (level == MaximumLevel || node->hash == hash) {
            node->values = insert_value(existing_values, key, &value);
            ++(*size);
            return node;
        } else {
            struct Node *new_node = allocate(sizeof(struct Node));
            size_t existing_node_index = level_index(node->hash, level);
            if (existing_node_index != index) {
                new_node->sub_nodes[existing_node_index] = node;
                new_node->sub_nodes[index] = create_value_node(key, hash, value);
                ++(*size);
                return new_node;
            } else {
                new_node->sub_nodes[existing_node_index] = Node_put(node, level + 1, key, hash, value, size);
                return new_node;
            }
        }
    } else {
        struct Node *existing_node = node->sub_nodes[index];
        if (existing_node) {
            node->sub_nodes[index] = Node_put(existing_node, level + 1, key, hash, value, size);
        } else {
            node->sub_nodes[index] = create_value_node(key, hash, value);
            ++(*size);
            return node;
        }
        return node;
    }
}

void HashMap_put(struct HashMap *self, Key key, struct Any value) {
    if (self->root) {
        self->root = Node_put(self->root, 0, key, calculate_hash(key), value, &self->size);
    } else {
        self->root = create_value_node(key, calculate_hash(key), value);
        self->size = 1;
    }
}

Value Node_get(struct Node *node, uint8_t level, Key key, size_t hash) {
    if (node->is_value_node) {
        struct ValueList *values = node->values;
        while (values) {
            if (values->key == key) {
                return values->value;
            }
            values = values->next;
        }
        return Undefined();
    } else {
        size_t index = level_index(hash, level);
        if (node->sub_nodes[index]) {
            return Node_get(node->sub_nodes[index], level + 1, key, hash);
        } else {
            return Undefined();
        }
    }
}

Value HashMap_get(struct HashMap *self, Key key) {
    if (self->root) {
        return Node_get(self->root, 0, key, calculate_hash(key));
    } else {
        return Undefined();
    }
}

struct Node *Node_remove(struct Node *node, uint8_t level, Key key, size_t hash, bool *found) {
    if (node->is_value_node) {
        struct ValueList *values = node->values;
        struct ValueList **source = &node->values;
        while (values) {
            if (values->key == key) {
                if (*found) {
                    fail("Found multiple entries for key %zu", key);
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
            node->sub_nodes[index] = Node_remove(node->sub_nodes[index], level + 1, key, hash, found);
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

bool HashMap_remove(struct HashMap *self, Key key) {
    if (self->root) {
        bool found = false;
        self->root = Node_remove(self->root, 0, key, calculate_hash(key), &found);
        if (found) {
            --self->size;
        }
        return found;
    } else {
        return false;
    }
}
