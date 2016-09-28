#include "sema.h"
#include "ast.h"
#include "allocator.h"
#include "symbol_table.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SEMA_ERROR() ((void)0)

struct sema_state
{
    symbol_table_t *current_table;
    ast_t          *current_function;
    ast_t          *last_type;
};

sema_state_t *sema_state_init(allocator_t *alloc)
{
    sema_state_t *state     = allocator_new(alloc, sizeof(sema_state_t));
    state->current_table    = symbol_table_init(NULL);
    state->current_function = NULL;
    return state;
}

void check_type(ast_t *ast, sema_state_t *state)
{
    //NOT_IMPLEMENTED();
}

void can_convert_to(ast_t *from_type, ast_t *to_type, sema_state_t *state)
{
    //NOT_IMPLEMENTED();
}

void check_is_bool_convertable(ast_t *from_type, sema_state_t *state)
{
    //NOT_IMPLEMENTED();
}

ast_t *check_operation_is_supported(enum OPERATOR op, ast_t *lhs_type, ast_t *rhs_type, sema_state_t *state)
{
    //NOT_IMPLEMENTED();
}

void sema_FUNCTION(ast_t *ast, sema_state_t *state)
{
    symbol_table_insert(state->current_table, ast->function.name, ast);
    check_type(ast->function.return_, state);
    ast_t *temp = state->current_function;
    state->current_function = ast;
    sema(ast->function.params, state);
    sema(ast->function.statements, state);
    state->current_function = temp;
}

void sema_TYPE_DECL(ast_t *ast, sema_state_t *state)
{
    assert(ast->type == AST_TYPE_TYPE_DECL);
    assert(strcmp(ast->string, "int") == 0);
    //NOT_IMPLEMENTED()
}

void sema_PARAM(ast_t *ast, sema_state_t *state)
{
    check_type(ast->param.type, state);
    symbol_table_insert(state->current_table, ast->param.name, ast->param.type);
}

void sema_LIST(ast_t *ast, sema_state_t *state)
{
    for (size_t i = 0; i < ast->list.count; i++)
    {
        sema(ast->list.data[i], state);
    }
}

void sema_STATEMENT_LIST(ast_t *ast, sema_state_t *state)
{
    for (size_t i = 0; i < ast->list.count; i++)
    {
        sema(ast->list.data[i], state);
    }
}

void sema_RETURN(ast_t *ast, sema_state_t *state)
{
    sema(ast->return_.expr, state);
    can_convert_to(state->last_type, state->current_function->function.return_, state);
}

void sema_ID(ast_t *ast, sema_state_t *state)
{
    state->last_type = symbol_table_get(state->current_table, ast);
}

void sema_INT_LIT(ast_t *ast, sema_state_t *state)
{
    state->last_type = ast;
}

void sema_VAR_DECL(ast_t *ast, sema_state_t *state)
{
    check_type(ast->var_decl.type, state);
    sema(ast->var_decl.expr, state);
    symbol_table_insert(state->current_table, ast->var_decl.name, ast->var_decl.type);
}

void sema_WHILE(ast_t *ast, sema_state_t *state)
{
    sema(ast->while_.cond, state);
    check_is_bool_convertable(state->last_type, state);
    sema(ast->while_.stmt, state);
}

void sema_IF(ast_t *ast, sema_state_t *state)
{
    sema(ast->if_.cond, state);
    check_is_bool_convertable(state->last_type, state);
    sema(ast->if_.success, state);
    sema(ast->if_.fail, state);
}

void sema_BINARY_OPERATION(ast_t *ast, sema_state_t *state)
{
    sema(ast->binary_op.arg_0, state);
    ast_t *lhs_type = state->last_type;
    sema(ast->binary_op.arg_1, state);
    ast_t *rhs_type = state->last_type;
    state->last_type = check_operation_is_supported(ast->binary_op.type, lhs_type, rhs_type, state);
}

void sema_UNARY_OPERATION(ast_t *ast, sema_state_t *state)
{
    NOT_IMPLEMENTED();
}

void sema_BUILTIN_FUNC_CALL(ast_t *ast, sema_state_t *state)
{
    NOT_IMPLEMENTED();
}

void sema_FUNC_CALL(ast_t *ast, sema_state_t *state)
{
    sema(ast->func_call.func, state);
    ast_t *function = state->last_type;

    if (function->function.params->list.count != ast->func_call.params->list.count)
    {
        SEMA_ERROR();
    }

    for (size_t i = 0; i < ast->func_call.params->list.count; i++)
    {
        sema(ast->func_call.params->list.data[i], state);
        can_convert_to(state->last_type, function->function.params->list.data[i], state);
    }
}

void sema_QUALIFIER_ARRAY(ast_t *ast, sema_state_t *state)
{
    NOT_IMPLEMENTED();
}

void sema_SCOPE(ast_t *ast, sema_state_t *state)
{
    symbol_table_t *parent = state->current_table;
    state->current_table = symbol_table_init(parent);
    ast->scope.table     = state->current_table;
    sema(ast->scope.child, state);
    state->current_table = parent;
}

void sema(ast_t *ast, sema_state_t *state)
{
#define X(NAME)                           \
    if(ast->type == AST_TYPE_ ## NAME)    \
    {                                     \
        sema_ ## NAME(ast, state);        \
        return;                           \
    }
    X_AST_TYPE_LIST
#undef X
    TODO_ERROR_HANDLING("Encountered node with unknown type.\n");
}