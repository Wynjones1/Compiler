#include "ast.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "parser.h"
#include "allocator.h"

#define AST_CHECK_TYPE(AST, TYPE) \
    assert(AST->type== AST_TYPE_ ## TYPE);

ast_t *ast_list(size_t count, ast_t **data, parse_state_t *ps)
{
    ast_t *out = allocator_new(ps->al, sizeof(ast_t));
    out->type  = AST_TYPE_LIST;
    out->list.count = count;
    out->list.data = allocator_new(ps->al, sizeof(ast_t*) * count);
    memcpy(out->list.data, data, sizeof(ast_t*) * count);
    return out;
}

ast_t *ast_make(enum AST_TYPE type, parse_state_t *ps)
{
    
    ast_t *out;
    if(ps == NULL)
    {
        out = malloc(sizeof(ast_t));
    }
    else
    {
        out = allocator_new(ps->al, sizeof(ast_t));
    }
    out->type = type;
    return out;
}

const char *ast_type_string(enum AST_TYPE type)
{
#define X(NAME) if(type == AST_TYPE_ ## NAME) return #NAME;
    X_AST_TYPE_LIST
#undef X
    return "(UNKNOWN)";
}
