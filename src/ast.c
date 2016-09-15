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

ast_t *ast_make(enum AST_TYPE type, allocator_t *al)
{
    
    ast_t *out;
    if(al == NULL)
    {
        out = malloc(sizeof(ast_t));
    }
    else
    {
        out = allocator_new(al, sizeof(ast_t));
    }
    out->type = type;
    return out;
}

void ast_list_append(ast_t *list, ast_t *elem)
{
    AST_CHECK_TYPE(list, LIST);
    list->list.count++;
    list->list.data = realloc(list->list.data, sizeof(ast_t*) * list->list.count);
    list->list.data[list->list.count - 1] = elem;
}

const char *const ast_type_string(enum AST_TYPE type)
{
#define X(NAME) if(type == AST_TYPE_ ## NAME) return #NAME;
    X_AST_TYPE_LIST
#undef X
    return "(UNKNOWN)";
}
