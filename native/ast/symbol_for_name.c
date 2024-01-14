#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "symbol_for_name.h"

struct Node {
    const char *key;
    const struct Symbol *value;
    struct Node *left;
    struct Node *right;
};

struct SymbolForName {
    struct Node *baseNode;
};

struct SymbolForName *SymbolForName_create(void) {
    struct SymbolForName *result = malloc(sizeof(struct SymbolForName));
    bzero(result, sizeof(struct SymbolForName));
    return result;
}

void deleteNode(struct Node *pNode);

void SymbolForName_delete(struct SymbolForName *instance) {
    deleteNode(instance->baseNode);
    free(instance);
}

void insert(struct Node *node, const char *key, const struct Symbol *value);

struct Node *createNode(const char *key, const struct Symbol *value);

void SymbolForName_set(struct SymbolForName *self, const char *key, const struct Symbol *value) {
    if (self->baseNode == NULL) {
        self->baseNode = createNode(key, value);
    } else {
        insert(self->baseNode, key, value);
    }
}

const struct Symbol *replacementForNode(struct Node *node, const char *key) {
    if (node == NULL) {
        struct Symbol *result = malloc(sizeof(struct Symbol));
        memcpy(result,
               &(struct Symbol) {
                       key,
                       sizeof(key)
               },
               sizeof(struct Symbol));
        return result;
    }
    int delta = strcmp(key, node->key);
    if (delta == 0) {
        return node->value;
    } else if (delta < 0) {
        return replacementForNode(node->left, key);
    } else {
        return replacementForNode(node->right, key);
    }
}

void insert(struct Node *node, const char *key, const struct Symbol *value) {
    int delta = strcmp(key, node->key);
    if (delta == 0) {
        node->value = value;
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

struct Node *createNode(const char *key, const struct Symbol *value) {
    struct Node *result = malloc(sizeof(struct Node));
    bzero(result, sizeof(struct Node));
    result->key = key;
    result->value = value;
    return result;
}
