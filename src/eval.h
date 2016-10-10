#ifndef EVAL_H
#define EVAL_H
#include "ast.h"
#include "allocator.h"

typedef struct eval_state eval_state_t;

typedef struct eval_result
{
    const char *stderr_;
    const char *stdout_;
    uint32_t    retcode;
}eval_result_t;

eval_state_t  *eval_state_init(void);
ast_t         *eval(ast_t *ast, eval_state_t *al);
ast_t         *make_entry_node(void);
eval_result_t *eval_string(const char *string);

#endif
