#include "pointer_set.h"

#include <stdbool.h>
#include <core/allocate.h>
#include <core/errors.h>
#include <stdint.h>

static const uint8_t MaximumLevel = 6;

struct Node {
    bool is_value_node;
    union {
        struct Node *sub_nodes[32];
        struct {
            Element value;
        };
    };
};

struct PointerSet *PointerSet_create(void) {
    return allocate_unmanaged(sizeof(struct PointerSet));
}

static void delete_node(struct Node *node) {
    if (!node->is_value_node) {
        for (size_t index = 0; index < 32; ++index) {
            if (node->sub_nodes[index]) {
                delete_node(node->sub_nodes[index]);
            }
        }
    }
    deallocate_unmanaged(node);
}

void PointerSet_delete(struct PointerSet *instance) {
    if (instance->root) {
        delete_node(instance->root);
    }
    deallocate_unmanaged(instance);
}

static size_t level_index(Element value, uint8_t level) {
    if (5 * level >= 32) {
        fail_with_message("PointerSet: illegal level: %d", (int) level);
    }
    return (value >> ((size_t) 5 * level)) % 0x20;
}

static struct Node *create_value_node(Element value) {
    struct Node *new_node = allocate_unmanaged(sizeof(struct Node));
    new_node->is_value_node = true;
    new_node->value = value;
    return new_node;
}

static struct Node *Node_add(struct Node *node, uint8_t level, Element value, size_t *size) {
    size_t index = level_index(value, level);
    if (node->is_value_node) {
        if (node->value == value) {
            return node;
        } else if (level == MaximumLevel) {
            fail_with_message("Logical error");
        } else {
            struct Node *new_node = allocate_unmanaged(sizeof(struct Node));
            size_t existing_node_index = level_index(node->value, level);
            if (existing_node_index != index) {
                new_node->sub_nodes[existing_node_index] = node;
                new_node->sub_nodes[index] = create_value_node(value);
                ++(*size);
                return new_node;
            } else {
                new_node->sub_nodes[existing_node_index] = Node_add(node, level + 1, value, size);
                return new_node;
            }
        }
    } else {
        struct Node *existing_node = node->sub_nodes[index];
        if (existing_node) {
            node->sub_nodes[index] = Node_add(existing_node, level + 1, value, size);
        } else {
            node->sub_nodes[index] = create_value_node(value);
            ++(*size);
            return node;
        }
        return node;
    }
}

void PointerSet_add(struct PointerSet *self, Element value) {
    if (self->root) {
        self->root = Node_add(self->root, 0, value, &self->size);
    } else {
        self->root = create_value_node(value);
        self->size = 1;
    }
}

static bool Node_contains(struct Node *node, uint8_t level, Element value) {
    if (node->is_value_node) {
        if (node->value == value) {
            return true;
        } else {
            return false;
        }
    } else {
        size_t index = level_index(value, level);
        if (node->sub_nodes[index]) {
            return Node_contains(node->sub_nodes[index], level + 1, value);
        } else {
            return false;
        }
    }
}

bool PointerSet_contains(struct PointerSet *self, Element value) {
    if (self->root) {
        return Node_contains(self->root, 0, value);
    } else {
        return false;
    }
}

static struct Node *Node_remove(struct Node *node, uint8_t level, Element value, bool *found) {
    if (node->is_value_node) {
        deallocate_unmanaged(node);
        return 0;
    } else {
        size_t index = level_index(value, level);
        if (node->sub_nodes[index]) {
            node->sub_nodes[index] = Node_remove(node->sub_nodes[index], level + 1, value, found);
            for (size_t search_index = 0; search_index < 32; ++search_index) {
                if (node->sub_nodes[search_index]) {
                    return node;
                }
            }
            deallocate_unmanaged(node);
            return 0;
        } else {
            return node;
        }
    }
}

bool PointerSet_remove(struct PointerSet *self, Element value) {
    if (self->root) {
        bool found = false;
        self->root = Node_remove(self->root, 0, value, &found);
        if (found) {
            --self->size;
        }
        return found;
    } else {
        return false;
    }
}
