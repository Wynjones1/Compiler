#include "intermediate.h"
#include "ast.h"
#include "helpers.h"
#include <stdio.h>

struct intermediate_gen_state
{
    int x;
};

#define X_INTERMEDIATE_TYPE_LIST\
    X(LOAD) \
    X(STORE) \
    X(OPERATION) \
    X(RETURN) \

struct intermediate
{
    union
    {
        struct
        {
            const char *symbol;
            uint32_t temp;
        }load;

        struct
        {
            const char *symbol;
            uint32_t temp;
        }store;

        struct
        {
            enum OPERATOR type;
            uint32_t lhs_temp;
            uint32_t rhs_temp;
            uint32_t result_temp;
        }operation;

        uint32_t return_;
    };
};

struct basic_block
{
    size_t           count;
    intermediate_t **data;
};

intermediate_gen_state_t *intermediate_state_init()
{

}

void intermediate_gen_FUNCTION(ast_t *ast, intermediate_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

void intermediate_gen_TYPE_DECL(ast_t *ast, intermediate_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

void intermediate_gen_PARAM(ast_t *ast, intermediate_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

void intermediate_gen_LIST(ast_t *ast, intermediate_gen_state_t *state)
{
    for (size_t i = 0; i < ast->list.count; i++)
    {
        intermediate_gen(ast->list.data[i], state);
    }
}

void intermediate_gen_STATEMENT_LIST(ast_t *ast, intermediate_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

void intermediate_gen_RETURN(ast_t *ast, intermediate_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

void intermediate_gen_ID(ast_t *ast, intermediate_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

void intermediate_gen_INT_LIT(ast_t *ast, intermediate_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

void intermediate_gen_VAR_DECL(ast_t *ast, intermediate_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

void intermediate_gen_WHILE(ast_t *ast, intermediate_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

void intermediate_gen_IF(ast_t *ast, intermediate_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

void intermediate_gen_BINARY_OPERATION(ast_t *ast, intermediate_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

void intermediate_gen_UNARY_OPERATION(ast_t *ast, intermediate_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

void intermediate_gen_BUILTIN_FUNC_CALL(ast_t *ast, intermediate_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

void intermediate_gen_FUNC_CALL(ast_t *ast, intermediate_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

void intermediate_gen_QUALIFIER_ARRAY(ast_t *ast, intermediate_gen_state_t *state)
{
	NOT_IMPLEMENTED();
}

void intermediate_gen_SCOPE(ast_t *ast, intermediate_gen_state_t *state)
{
    intermediate_gen(ast->scope.child, state);
}


intermediate_t *intermediate_gen(ast_t *ast, intermediate_gen_state_t *state)
{
#define X(NAME)                                \
    if(ast->type == AST_TYPE_ ## NAME)         \
    {                                          \
        intermediate_gen_ ## NAME(ast, state); \
        return;                                \
    }
    X_AST_TYPE_LIST
#undef X
    TODO_ERROR_HANDLING("Encountered node with unknown type.\n");
}