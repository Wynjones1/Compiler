#ifndef INTERMEDIATE_H
#define INTERMEDIATE_H
#include "ast.h"
#include "helpers.h"
#include <stdio.h>

typedef struct ir_gen_state ir_gen_state_t;
typedef struct ast          ast_t;
typedef struct ir           ir_t;

#define X_INTERMEDIATE_TYPE_LIST\
    X(LOAD) \
    X(STORE) \
    X(OPERATION) \
    X(RETURN) \

struct ir
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
    size_t   count;
    ir_t   **data;
};

ir_gen_state_t *intermediate_state_init();
ir_t *ir_gen(ast_t *ast, ir_gen_state_t *state);
#endif
