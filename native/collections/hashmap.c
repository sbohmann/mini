#include "hashmap.h"

#include <stdbool.h>
#include <stdlib.h>

#include <core/allocate.h>
#include <core/errors.h>
#include <core/typedefs.h>

static const uint8_t MaximumLevel = 6;

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
            Hash hash;
            struct ValueList *values;
        };
    };
};

void HashMap_destructor(struct HashMap *instance);

struct HashMap *HashMap_create() {
    struct HashMap *result = allocate(sizeof(struct HashMap));
    Complex_init(&result->base);
    result->base.destructor = (void (*)(struct ComplexValue *)) HashMap_destructor;
    result->base.type = MapComplexType;
    return result;
}

static void delete_values(struct ValueList *values) {
    if (values->next) {
        delete_values(values->next);
    }
    deallocate(values);
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
    deallocate(node);
}

void HashMap_destructor(struct HashMap *instance) {
    if (instance->root) {
        delete_node(instance->root);
    }
}

void HashMap_release(struct HashMap *instance) {
    release(&instance->base);
}

static size_t level_index(Hash hash, uint8_t level) {
    if (5 * level >= 32) {
        fail_with_message("HashMap: illegal level: %d", (int) level);
    }
    return (hash >> (5 * level)) % 0x20;
}

static bool replace_value(struct ValueList *values, Key key, Value value) {
    while (values) {
        if (Any_raw_equal(values->key, key)) {
            Any_release(values->value);
            values->value = value;
            return true;
        }
        values = values->next;
    }
    return false;
}

static struct ValueList *insert_value(struct ValueList *existing_values, Key key, Value *value) {
    struct ValueList *new_values = allocate(sizeof(struct ValueList));
    new_values->key = key;
    new_values->value = (*value);
    new_values->next = existing_values;
    return new_values;
}

static struct Node *create_value_node(Key key, Hash hash, Value value) {
    struct Node *new_node = allocate(sizeof(struct Node));
    new_node->is_value_node = true;
    new_node->hash = hash;
    struct ValueList *values = allocate(sizeof(struct ValueList));
    values->key = key;
    values->value = value;
    new_node->values = values;
    return new_node;
}

static struct Node *Node_put(struct Node *node, uint8_t level, Key key, Hash hash, Value value, size_t *size) {
    size_t index = level_index(hash, level);
    if (node->is_value_node) {
        struct ValueList *existing_values = node->values;
        if (replace_value(existing_values, key, value)) {
            return node;
        } else if (node->hash == hash) {
//            printf("Collision!!!");
            node->values = insert_value(existing_values, key, &value);
            ++(*size);
            return node;
        } else if (level == MaximumLevel) {
            fail_with_message("Logical error");
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

void HashMap_put(struct HashMap *self, Key key, Value value) {
    if (self->root) {
        self->root = Node_put(self->root, 0, key, Any_hash(key), value, &self->size);
    } else {
        self->root = create_value_node(key, Any_hash(key), value);
        self->size = 1;
    }
}

static bool Node_set(struct Node *node, uint8_t level, Key key, Hash hash, Value value) {
    if (node->is_value_node) {
        struct ValueList *values = node->values;
        while (values) {
            if (Any_raw_equal(values->key, key)) {
                values->value = value;
                return true;
            }
            values = values->next;
        }
        return false;
    } else {
        size_t index = level_index(hash, level);
        if (node->sub_nodes[index]) {
            return Node_set(node->sub_nodes[index], level + 1, key, hash, value);
        } else {
            return false;
        }
    }
}

bool HashMap_set(struct HashMap *self, Key key, Value value) {
    if (self->root) {
        return Node_set(self->root, 0, key, Any_hash(key), value);
    } else {
        return false;
    }
}

static struct MapResult Node_get(struct Node *node, uint8_t level, Key key, Hash hash) {
    if (node->is_value_node) {
        struct ValueList *values = node->values;
        while (values) {
            if (Any_raw_equal(values->key, key)) {
                return (struct MapResult) {true, values->value};
            }
            values = values->next;
        }
        return (struct MapResult) {false, None()};
    } else {
        size_t index = level_index(hash, level);
        if (node->sub_nodes[index]) {
            return Node_get(node->sub_nodes[index], level + 1, key, hash);
        } else {
            return (struct MapResult) {false, None()};
        }
    }
}

struct MapResult HashMap_get(struct HashMap *self, Key key) {
    if (self->root) {
        return Node_get(self->root, 0, key, Any_hash(key));
    } else {
        return (struct MapResult) {false, None()};
    }
}

static struct Node *Node_remove(struct Node *node, uint8_t level, Key key, Hash hash, bool *found) {
    if (node->is_value_node) {
        struct ValueList *values = node->values;
        struct ValueList **source = &node->values;
        while (values) {
            if (Any_raw_equal(values->key, key)) {
                if (*found) {
                    fail_with_message("Found multiple entries for key %zu", key);
                }
                struct ValueList *next = values->next;
                deallocate(values);
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
            deallocate(node);
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
            deallocate(node);
            return 0;
        } else {
            return node;
        }
    }
}

bool HashMap_remove(struct HashMap *self, Key key) {
    if (self->root) {
        bool found = false;
        self->root = Node_remove(self->root, 0, key, Any_hash(key), &found);
        if (found) {
            --self->size;
        }
        return found;
    } else {
        return false;
    }
}

void Node_foreach(struct Node *node, HashMapForeachFunction function, void *context) {
    if (node->is_value_node) {
        struct ValueList *list = node->values;
        while (list) {
            function(list->key, list->value, context);
            list = list->next;
        }
    } else {
        for (size_t index = 0; index < 32; ++index) {
            if (node->sub_nodes[index]) {
                Node_foreach(node->sub_nodes[index], function, context);
            }
        }
    }
}

void HashMap_foreach(struct HashMap *self, HashMapForeachFunction function, void *context) {
    if (self->root) {
        Node_foreach(self->root, function, context);
    }
}
