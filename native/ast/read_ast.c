#include <string.h>

#include "read_ast.h"
#include "minic/elements/element_queue.h"

void read_statements(struct ElementQueue *queue);

void read_variable_declaration(struct ElementQueue *queue, bool constant);

void read_function_declaration(struct ElementQueue *pQueue, bool b);

void read_ast(struct ParsedModule *module) {
    struct ElementQueue *queue = ElementQueue_create(module->elements);
    read_statements(queue);
}

void read_statements(struct ElementQueue *queue) {
    const struct Element *firstElement = ElementQueue_peek(queue);
    if (firstElement->type == TokenElement) {
        const struct Token *token = firstElement->token;
        if (token->type == SymbolToken) {
            if (strcmp("var", token->text->value) == 0) {
                ElementQueue_next(queue);
                read_variable_declaration(queue, false);
            } else if (strcmp("let", token->text->value) == 0) {
                ElementQueue_next(queue);
                read_variable_declaration(queue, true);
            } else if (strcmp("function", token->text->value) == 0) {
                ElementQueue_next(queue);
                read_function_declaration(queue, false);
            }
        }
    }
}

void read_variable_declaration(struct ElementQueue *queue, bool constant) {

}

void read_function_declaration(struct ElementQueue *pQueue, bool b) {

}
