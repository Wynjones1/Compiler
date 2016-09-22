#include "ast.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "parser.h"
#include "allocator.h"
#include "string_common.h"

#define AST_CHECK_TYPE(AST, TYPE) \
    assert(AST->type== AST_TYPE_ ## TYPE);


static ast_t *ast_make(enum AST_TYPE type, parse_state_t *ps)
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

ast_t *ast_function(ast_t *name, ast_t *params, ast_t *return_, ast_t *stmts, parse_state_t *ps)
{
	ast_t *out = ast_make(AST_TYPE_FUNCTION, ps);
    out->function.name = name;
    out->function.params = params;
    out->function.return_ = return_;
    out->function.statements = stmts;
	return out;
}

ast_t *ast_list(size_t count, ast_t **data, parse_state_t *ps)
{
    ast_t *out = allocator_new(ps->al, sizeof(ast_t));
    out->type  = AST_TYPE_LIST;
    out->list.count = count;
    out->list.data = allocator_new(ps->al, sizeof(ast_t*) * count);
    memcpy(out->list.data, data, sizeof(ast_t*) * count);
    return out;
}

ast_t *ast_param(ast_t *type, ast_t *name, parse_state_t *ps)
{
	ast_t *out = ast_make(AST_TYPE_PARAM, ps);
    out->param.type = type;
    out->param.name = name;
	return out;
}

ast_t *ast_vardecl(ast_t *type, ast_t *name, ast_t *expr, parse_state_t *ps)
{
	ast_t *out = ast_make(AST_TYPE_VAR_DECL, ps);
    out->vardecl.type = type;
    out->vardecl.name = name;
    out->vardecl.expr = expr;
	return out;
}

ast_t *ast_binary_op(enum OPERATOR type, parse_state_t *ps)
{
	ast_t *out = ast_make(AST_TYPE_BINARY_OPERATION, ps);
    out->binary_op.type  = type;
    out->binary_op.arg_0 = NULL;
    out->binary_op.arg_1 = NULL;
	return out;
}

ast_t *ast_unary_op(enum OPERATOR type, parse_state_t *ps)
{
	ast_t *out = ast_make(AST_TYPE_UNARY_OPERATION, ps);
    out->unary_op.type = type;
    out->unary_op.arg  = NULL;
	return out;
}

ast_t *ast_return(ast_t *expr, parse_state_t *ps)
{
	ast_t *out = ast_make(AST_TYPE_RETURN, ps);
    out->return_.expr = expr;
	return out;
}

ast_t *ast_if(ast_t *cond, ast_t *success, ast_t *fail, parse_state_t *ps)
{
	ast_t *out = ast_make(AST_TYPE_IF, ps);
    out->if_.cond = cond;
    out->if_.success = success;
    out->if_.fail = fail;
	return out;
}

ast_t *ast_while(ast_t *cond, ast_t *stmts, parse_state_t *ps)
{
	ast_t *out = ast_make(AST_TYPE_WHILE, ps);
    out->while_.cond = cond;
    out->while_.stmt = stmts;
	return out;
}

ast_t *ast_func_call(ast_t *func, ast_t *params, parse_state_t *ps)
{
	ast_t *out = ast_make(AST_TYPE_FUNC_CALL, ps);
    out->func_call.func = func;
    out->func_call.params = params;
	return out;
}

ast_t *ast_decl(typedecl_t *decl, parse_state_t *ps)
{
	ast_t *out = ast_make(AST_TYPE_TYPE_DECL, ps);
    out->decl = decl;
	return out;
}

ast_t *ast_int_literal(const char *value, parse_state_t *ps)
{
    ast_t *out = ast_make(AST_TYPE_INT_LIT, ps);
    out->string = string_copy(value, ps->al);
    return out;
}

ast_t *ast_id(const char *value, parse_state_t *ps)
{
    ast_t *out = ast_make(AST_TYPE_ID, ps);
    out->string = string_copy(value, ps->al);
    return out;
}
