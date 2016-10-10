#ifndef AST_H
#define AST_H
#include <stddef.h>
#include <stdbool.h>
#include "allocator.h"
#include "lexer.h"
#include "operators.h"

#define X_AST_TYPE_LIST  \
    X(ROOT)              \
    X(FUNCTION)          \
    X(TYPE_DECL)         \
    X(PARAM)             \
    X(LIST)              \
    X(STATEMENT_LIST)    \
    X(RETURN)            \
    X(ID)                \
    X(INT_LIT)           \
    X(VAR_DECL)          \
    X(WHILE)             \
    X(IF)                \
    X(BINARY_OPERATION)  \
    X(UNARY_OPERATION)   \
    X(BUILTIN_FUNC_CALL) \
    X(FUNC_CALL)         \
    X(QUALIFIER_ARRAY)   \
    X(SCOPE)             \

#define X_BUILTIN_FUNC_LIST \
    X("print")

enum AST_TYPE
{
#define X(NAME) AST_TYPE_ ## NAME,
    X_AST_TYPE_LIST
#undef X
    NUM_AST_TYPES,
};

typedef struct ast ast_t;
typedef struct allocator allocator_t;
typedef struct symbol_table symbol_table_t;

struct ast
{
    enum AST_TYPE type;
    union
    {
        struct
        {
            allocator_t *al;
            ast_t       *child;
        }root;

        struct
        {
            ast_t *name;
            ast_t *params;
            ast_t *return_;
            ast_t *statements;
        }function;

        struct
        {
            size_t  count;
            ast_t **data;
        }list;

        struct
        {
            ast_t *type;
            ast_t *name;
        }param;

        struct
        {
            ast_t *type;
            ast_t *name;
            ast_t *expr;
        }var_decl;

        struct
        {
            enum OPERATOR type;
            ast_t *arg_0;
            ast_t *arg_1;
        }binary_op;

        struct
        {
            enum OPERATOR type;
            ast_t *arg;
        }unary_op;

        struct
        {
            ast_t *expr;
        }return_;

        struct
        {
            ast_t *cond;
            ast_t *success;
            ast_t *fail;
        }if_;

        struct
        {
            ast_t *cond;
            ast_t *stmt;
        }while_;

        struct
        {
            ast_t *func;
            ast_t *params;
        }func_call;
        
        struct
        {
            ast_t *array_size;
            ast_t *next;
        }type_decl;

        struct
        {
            symbol_table_t *table;
            ast_t          *child;
        }scope;

        const char *string;
    };
};

ast_t *ast_function(ast_t *name, ast_t *params, ast_t *return_, ast_t *stmts, allocator_t *al);
ast_t *ast_list(size_t count, ast_t **data, allocator_t *al);
ast_t *ast_param(ast_t *type, ast_t *name, allocator_t *al);
ast_t *ast_vardecl(ast_t *type, ast_t *name, ast_t *expr, allocator_t *al);
ast_t *ast_binary_op(enum OPERATOR type, allocator_t *al);
ast_t *ast_unary_op(enum OPERATOR type, allocator_t *al);
ast_t *ast_return(ast_t *expr, allocator_t *al);
ast_t *ast_if(ast_t *cond, ast_t *success, ast_t *fail, allocator_t *al);
ast_t *ast_while(ast_t *cond, ast_t *stmts, allocator_t *al);
ast_t *ast_func_call(ast_t *func, ast_t *params, allocator_t *al);
ast_t *ast_int_literal(const char *value, allocator_t *al);
ast_t *ast_id(const char *value, allocator_t *al);
ast_t *ast_array_qualifier(ast_t *type, ast_t *size, allocator_t *al);
ast_t *ast_scope(ast_t *child, allocator_t *al);
ast_t *ast_root(void);

/* Return a string corresponding to the AST type.
   Parameters:
        type - ast type for which to return the string

   Returns:
        string representing the ast type.
*/
const char *ast_type_string(enum AST_TYPE type);

#endif
