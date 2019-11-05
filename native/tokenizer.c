#include "tokenizer.h"

#include "allocate.h"
#include "generated/token_list.h"

static struct Tokens *read_tokens(const struct Source *pSource) {
    struct TokenList *tokens = TokenList_create();
    struct Token *token = allocate(sizeof(struct Token));
    token->text = "one";
    TokenList_append(tokens, token);
    token = allocate(sizeof(struct Token));
    token->text = "two";
    TokenList_append(tokens, token);
    struct TokenListElement *iterator = TokenList_begin(tokens);
    while (iterator) {
        printf("token value: %s\n", TokenListIterator_get(iterator)->text);
        iterator = TokenListIterator_next(iterator);
    }
    iterator = TokenList_end(tokens);
    while (iterator) {
        printf("token value: %s\n", TokenListIterator_get(iterator)->text);
        iterator = TokenListIterator_previous(iterator);
    }
    TokenList_delete(tokens);
    return 0;
}

struct ParsedModule *read_file(const char *path) {
    struct ParsedModule *result = allocate(sizeof(struct Token));
    result->path = path;
    result->source = read_source(path);
    result->tokens = read_tokens(result->source);
    return result;
}
