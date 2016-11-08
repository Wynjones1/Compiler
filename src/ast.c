#include "ast.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "parser.h"
#include "allocator.h"
#include "string_common.h"

#define AST_CHECK_TYPE(AST, TYPE) \
    assert(AST->type== AST_TYPE_ ## TYPE);


static ast_t *ast_make(enum AST_TYPE type, allocator_t *al)
{
    
    ast_t *out;
    if(al == NULL)
    {
        out = malloc(sizeof(ast_t));
    }
    else
    {
        out = allocator_new(al, sizeof(ast_t));
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

ast_t *ast_function(ast_t *name, ast_t *params, ast_t *return_, ast_t *stmts, allocator_t *al)
{
	ast_t *out = ast_make(AST_TYPE_FUNCTION, al);
    out->function.name = name;
    out->function.params = params;
    out->function.return_ = return_;
    out->function.statements = stmts;
	return out;
}

ast_t *ast_list(size_t count, ast_t **data, allocator_t *al)
{
    ast_t *out = allocator_new(al, sizeof(ast_t));
    out->type  = AST_TYPE_LIST;
    out->list.count = count;
    out->list.data = allocator_new(al, sizeof(ast_t*) * count);
    memcpy(out->list.data, data, sizeof(ast_t*) * count);
    return out;
}

ast_t *ast_param(ast_t *type, ast_t *name, allocator_t *al)
{
	ast_t *out = ast_make(AST_TYPE_PARAM, al);
    out->param.type = type;
    out->param.name = name;
	return out;
}

ast_t *ast_vardecl(ast_t *type, ast_t *name, ast_t *expr, allocator_t *al)
{
	ast_t *out = ast_make(AST_TYPE_VAR_DECL, al);
    out->var_decl.type = type;
    out->var_decl.name = name;
    out->var_decl.expr = expr;
	return out;
}

ast_t *ast_binary_op(enum OPERATOR type, allocator_t *al)
{
	ast_t *out = ast_make(AST_TYPE_BINARY_OPERATION, al);
    out->binary_op.type  = type;
    out->binary_op.arg_0 = NULL;
    out->binary_op.arg_1 = NULL;
	return out;
}

ast_t *ast_unary_op(enum OPERATOR type, allocator_t *al)
{
	ast_t *out = ast_make(AST_TYPE_UNARY_OPERATION, al);
    out->unary_op.type = type;
    out->unary_op.arg  = NULL;
	return out;
}

ast_t *ast_return(ast_t *expr, allocator_t *al)
{
	ast_t *out = ast_make(AST_TYPE_RETURN, al);
    out->return_.expr = expr;
	return out;
}

ast_t *ast_if(ast_t *cond, ast_t *success, ast_t *fail, allocator_t *al)
{
	ast_t *out = ast_make(AST_TYPE_IF, al);
    out->if_.cond = cond;
    out->if_.success = success;
    out->if_.fail = fail;
	return out;
}

ast_t *ast_while(ast_t *cond, ast_t *stmts, allocator_t *al)
{
	ast_t *out = ast_make(AST_TYPE_WHILE, al);
    out->while_.cond = cond;
    out->while_.stmt = stmts;
	return out;
}

ast_t *ast_func_call(ast_t *func, ast_t *params, allocator_t *al)
{
	ast_t *out = ast_make(AST_TYPE_FUNC_CALL, al);
    out->func_call.func = func;
    out->func_call.params = params;

    if(out->func_call.func->type == AST_TYPE_ID)
    {
    #define X(STRING)\
        if(strcmp(out->func_call.func->string, STRING) == 0)\
            out->type = AST_TYPE_BUILTIN_FUNC_CALL;
        X_BUILTIN_FUNC_LIST
    #undef X
    }
	return out;
}

ast_t *ast_int_literal(const char *value, allocator_t *al)
{
    ast_t *out = ast_make(AST_TYPE_INT_LIT, al);
    out->string = string_copy(value, al);
    return out;
}

ast_t *ast_id(const char *value, allocator_t *al)
{
    ast_t *out = ast_make(AST_TYPE_ID, al);
    out->string = string_copy(value, al);
    return out;
}

ast_t *ast_array_qualifier(ast_t *type, ast_t *size, allocator_t *al)
{
    ast_t *out = ast_make(AST_TYPE_QUALIFIER_ARRAY, al);
    out->type_decl.next = type;
    out->type_decl.array_size = size;
    return out;
}

ast_t *ast_scope(ast_t *child, allocator_t *al)
{
    ast_t *out = ast_make(AST_TYPE_SCOPE, al);
    out->scope.table = NULL;
    out->scope.child = child;
    return out;
}

ast_t *ast_root(allocator_t *al)
{
    ast_t *out = malloc(sizeof(ast_t));
    out->type       = AST_TYPE_ROOT;
    out->root.al    = al; 
    out->root.child = NULL;
    return out;
}
