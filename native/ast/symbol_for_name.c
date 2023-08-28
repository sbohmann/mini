#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "symbol_for_name.h"

struct Node {
    const char *key;
    const char *value;
    size_t length;
    struct Node *left;
    struct Node *right;
};

struct SymbolForName {
    struct Node *baseNode;
};

struct SymbolForName * SymbolForName_create() {
    struct SymbolForName *result = malloc(sizeof (struct SymbolForName));
    bzero(result, sizeof (struct SymbolForName));
    return result;
}

void deleteNode(struct Node *pNode);

void SymbolForName_delete(struct SymbolForName *instance) {
    deleteNode(instance->baseNode);
    free(instance);
}

void deleteNode(struct Node *node) {
    if (node == NULL) {
        return;
    }
    deleteNode(node->left);
    deleteNode(node->right);
    free(node);
}

void insert(struct Node *node, const char *key, const char *value);

struct Node *createNode(const char *key, const char *value);

void SymbolForName_add(struct SymbolForName *self, const char *key, const char *value) {
    if (self->baseNode == NULL) {
        self->baseNode = createNode(key, value);
    } else {
        insert(self->baseNode, key, value);
    }
}

struct Symbol replacementForNode(struct Node *pNode, const char *key);

struct Symbol SymbolForName_get(struct SymbolForName *self, const char *key) {
    return replacementForNode(self->baseNode, key);
}

struct Symbol replacementForNode(struct Node *node, const char *key) {
    if (node == NULL) {
        return (struct Symbol) {
                NULL,
                0
        };
    }
    int delta = strcmp(key, node->key);
    if (delta == 0) {
        return (struct Symbol) {
                node->value,
                node->length
        };
    } else if (delta < 0) {
        return replacementForNode(node->left, key);
    } else {
        return replacementForNode(node->right, key);
    }
}

void insert(struct Node *node, const char *key, const char *value) {
    int delta = strcmp(key, node->key);
    if (delta == 0) {
        node->value = value;
        node->length = strlen(value);
    } else if (delta < 0) {
        if (node->left != NULL) {
            insert(node->left, key, value);
        } else {
            node->left = createNode(key, value);
        }
    } else {
        if (node->right != NULL) {
            insert(node->right, key, value);
        } else {
            node->right = createNode(key, value);
        }
    }
}

struct Node *createNode(const char *key, const char *value) {
    struct Node *result = malloc(sizeof (struct Node));
    bzero(result, sizeof (struct Node));
    result->key = key;
    result->value = value;
    result->length = strlen(value);
    return result;
}
