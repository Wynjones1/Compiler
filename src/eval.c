#include "eval.h"
#include <stdio.h>
#include <stdlib.h>
#include "string_common.h"
#include "parser.h"
#include <assert.h>

typedef struct symbol_table symbol_table_t;

struct symbol_table
{
    struct
    {
        const char *id;
        ast_t      *node;
    }data[100];
    size_t size;
    symbol_table_t *parent;
};

symbol_table_t *symtab_init(symbol_table_t *parent)
{
    symbol_table_t *out = malloc(sizeof(symbol_table_t));
    out->size= 0;
    out->parent = parent;
    return out;
}

typedef struct eval_state
{
    symbol_table_t *table;
    allocator_t    *al;
}eval_state_t;

ast_t *query_table(symbol_table_t *table, const char *symbol)
{
    while(table)
    {
        for(size_t i = 0; i < table->size; i++)
        {
            if(string_equal(table->data[i].id, symbol))
            {
                return table->data[i].node;
            }
        }
        table = table->parent;
    }
    return NULL;
}

eval_state_t *eval_state_init(allocator_t *al)
{
    eval_state_t *out = malloc(sizeof(eval_state_t));
    out->table = symtab_init(NULL);
    out->al = al;
    return out;
}

ast_t *eval_FUNCTION(ast_t *ast, eval_state_t *state)
{}

ast_t *eval_TYPE_DECL(ast_t *ast, eval_state_t *state)
{}

ast_t *eval_PARAM(ast_t *ast, eval_state_t *state)
{}

ast_t *eval_LIST(ast_t *ast, eval_state_t *state)
{}

ast_t *eval_RETURN(ast_t *ast, eval_state_t *state)
{}

ast_t *eval_ID(ast_t *ast, eval_state_t *state)
{}

ast_t *eval_INT_LIT(ast_t *ast, eval_state_t *state)
{}

ast_t *eval_VAR_DECL(ast_t *ast, eval_state_t *state)
{}

ast_t *eval_WHILE(ast_t *ast, eval_state_t *state)
{}

ast_t *eval_IF(ast_t *ast, eval_state_t *state)
{}

ast_t *eval_BINARY_OPERATION(ast_t *ast, eval_state_t *state)
{}

ast_t *eval_UNARY_OPERATION(ast_t *ast, eval_state_t *state)
{}

ast_t *eval_FUNC_CALL(ast_t *ast, eval_state_t *state)
{
#if 0
    ast_t *id = ast->func_call.id;
    ast_t *func = query_table(state->table, id->id);
    ast_t *func_params = func->function.params;
    ast_t *call_params = ast->func_call.params;
    if(func_params->list.size != call_params->list.size)
    {
        fprintf(stderr, "Mismatch in number of parameters.\n");
    }

    eval_state_add_scope(state);
    for(int i = 0; i < func_params->list.size; i++)
    {
        ast_t *param = _eval(call_params->list.data[i], state);
        table_add_entry(func_params->list.data[i], param);
    }
#endif
}

ast_t *eval_(ast_t *ast, eval_state_t *state)
{
#define X(NAME)                           \
    if(ast->type == AST_TYPE_ ## NAME)    \
    {                                     \
        return eval_ ## NAME(ast, state); \
    }
    X_AST_TYPE_LIST
#undef X
    fprintf(stderr, "Encounterd node with unknown type.\n");
    exit(-1);
}

ast_t *make_entry_node(allocator_t *alloc)
{
    parse_state_t *ps = parse_state_init(NULL, alloc);
    return ast_func_call(ast_id("main", ps), ast_list(0, NULL, ps), ps);
}

ast_t *eval(ast_t *ast, allocator_t *al)
{
    printf("Starting to evaluate.\n");
    eval_state_t *state = eval_state_init(al);

    ast_t *entry_point = make_entry_node(al);

    return eval_(entry_point, state);
}