#include "intermediate.h"
#include "ast.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>

struct ir_gen_state
{
    int x;
};

ir_gen_state_t *intermediate_state_init()
{

}

ir_t * ir_gen_FUNCTION(ast_t *ast, ir_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

ir_t * ir_gen_TYPE_DECL(ast_t *ast, ir_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

ir_t * ir_gen_PARAM(ast_t *ast, ir_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

ir_t * ir_gen_LIST(ast_t *ast, ir_gen_state_t *state)
{
    for (size_t i = 0; i < ast->list.count; i++)
    {
        ir_gen(ast->list.data[i], state);
    }
}

ir_t * ir_gen_STATEMENT_LIST(ast_t *ast, ir_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

ir_t * ir_gen_RETURN(ast_t *ast, ir_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

ir_t * ir_gen_ID(ast_t *ast, ir_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

ir_t * ir_gen_INT_LIT(ast_t *ast, ir_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

ir_t * ir_gen_VAR_DECL(ast_t *ast, ir_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

ir_t * ir_gen_WHILE(ast_t *ast, ir_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

ir_t * ir_gen_IF(ast_t *ast, ir_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

ir_t * ir_gen_BINARY_OPERATION(ast_t *ast, ir_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

ir_t * ir_gen_UNARY_OPERATION(ast_t *ast, ir_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

ir_t * ir_gen_BUILTIN_FUNC_CALL(ast_t *ast, ir_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

ir_t * ir_gen_FUNC_CALL(ast_t *ast, ir_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

ir_t * ir_gen_QUALIFIER_ARRAY(ast_t *ast, ir_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

ir_t * ir_gen_SCOPE(ast_t *ast, ir_gen_state_t *state)
{
    ir_gen(ast->scope.child, state);
}

ir_t * ir_gen_ROOT(ast_t *ast, ir_gen_state_t *state)
{
    NOT_IMPLEMENTED();
}

ir_t *ir_gen(ast_t *ast, ir_gen_state_t *state)
{
#define X(NAME)                                       \
    if(ast->type == AST_TYPE_ ## NAME)                \
    {                                                 \
        return ir_gen_ ## NAME(ast, state); \
    }
    X_AST_TYPE_LIST
#undef X
    TODO_ERROR_HANDLING("Encountered node with unknown type.\n");
}
