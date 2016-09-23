#include "sema.h"
#include "ast.h"
#include "allocator.h"
#include <stdio.h>
#include <stdlib.h>

struct sema_state
{
    int reserved_;
};

sema_state_t *sema_state_init(allocator_t *alloc)
{
    sema_state_t *state = allocator_new(alloc, sizeof(sema_state_t));
    return state;
}

ast_t *sema(ast_t *ast, sema_state_t *state)
{
#define X(NAME)                           \
    if(ast->type == AST_TYPE_ ## NAME)    \
    {                                     \
        return sema_ ## NAME(ast, state); \
    }
    X_AST_TYPE_LIST
#undef X
    fprintf(stderr, "Encountered node with unknown type.\n");
    exit(-1);
}