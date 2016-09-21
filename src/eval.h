#ifndef EVAL_H
#define EVAL_H
#include "ast.h"
#include "allocator.h"

typedef struct eval_state eval_state_t;

eval_state_t *eval_state_init(allocator_t *al);
ast_t        *eval(ast_t *ast, eval_state_t *al);
ast_t        *make_entry_node(allocator_t *alloc);

void eval_string(const char *string);

#endif
