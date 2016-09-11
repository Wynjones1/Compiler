#include "ast.h"
#include <stdlib.h>
#include <assert.h>

#define AST_CHECK_TYPE(AST, TYPE) \
    assert(AST->type== AST_TYPE_ ## TYPE);

ast_t *ast_list()
{
    ast_t *out = malloc(sizeof(ast_t));
    out->type       = AST_TYPE_LIST;
    out->list.data  = NULL;
    out->list.count = 0;
    return out;
}

void ast_list_append(ast_t *list, ast_t *elem)
{
    AST_CHECK_TYPE(list, LIST);
    list->list.count++;
    list->list.data = realloc(list->list.data, sizeof(ast_t*) * list->list.count);
    list->list.data[list->list.count - 1] = elem;
}
