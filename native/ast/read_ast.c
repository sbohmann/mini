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
    StatementList *result = StatementList_create();
    while (true) {
        const struct Element *element = ElementQueue_next(queue);
        if (element == NULL) {
            return result;
        }
        if (element->type == TokenElement && element->token->type == SymbolToken) {
            const struct Token *token = element->token;
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
        } else {
            fail_at_position(element->position, "Expected a symbol");
        }
    }
}

void read_variable_declaration(struct ElementQueue *queue, bool constant) {
    const struct Element *nameElement = ElementQueue_take(queue);
    if (nameElement->type != TokenElement || nameElement->token->type != SymbolToken) {
        fail_after_position(nameElement->position, "Not a symbol");
    }
    const struct String *variableName = nameElement->token->value.string;
}

void read_function_declaration(struct ElementQueue *pQueue, bool b) {

}
