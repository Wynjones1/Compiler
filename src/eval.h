#ifndef EVAL_H
#define EVAL_H
#include "ast.h"
#include "allocator.h"

ast_t *eval(ast_t *ast, allocator_t *al);

#endif
