#ifndef AST_H
#define AST_H
#include <stddef.h>
#include "lexer.h"
#include "operators.h"

#define X_AST_TYPE_LIST \
    X(FUNCTION)         \
    X(TYPE_DECL)        \
    X(PARAM)            \
    X(LIST)             \
    X(STATEMENT_LIST)   \
    X(RETURN)           \
    X(ID)               \
    X(INT_LIT)          \
    X(VAR_DECL)         \
    X(WHILE)            \
    X(IF)               \
    X(BINARY_OPERATION) \
    X(UNARY_OPERATION)  \
    X(FUNC_CALL)

enum AST_TYPE
{
#define X(NAME) AST_TYPE_ ## NAME,
    X_AST_TYPE_LIST
#undef X
    NUM_AST_TYPES,
};


typedef struct ast ast_t;
typedef struct typedecl typedecl_t;
typedef struct parse_state parse_state_t;

struct typedecl
{
    ast_t *id;
};

struct ast
{
    enum AST_TYPE type;
    union
    {
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
        }vardecl;

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

        typedecl_t *decl;

        const char *string;
    };
};

ast_t *ast_function(ast_t *name, ast_t *params, ast_t *return_, ast_t *stmts, parse_state_t *ps);
ast_t *ast_list(size_t count, ast_t **data, parse_state_t *ps);
ast_t *ast_param(ast_t *type, ast_t *name, parse_state_t *ps);
ast_t *ast_vardecl(ast_t *type, ast_t *name, ast_t *expr, parse_state_t *ps);
ast_t *ast_binary_op(enum OPERATOR type, parse_state_t *ps);
ast_t *ast_unary_op(enum OPERATOR type, parse_state_t *ps);
ast_t *ast_return(ast_t *expr, parse_state_t *ps);
ast_t *ast_if(ast_t *cond, ast_t *success, ast_t *fail, parse_state_t *ps);
ast_t *ast_while(ast_t *cond, ast_t *stmts, parse_state_t *ps);
ast_t *ast_func_call(ast_t *func, ast_t *params, parse_state_t *ps);
ast_t *ast_decl(typedecl_t *type, parse_state_t *ps);
ast_t *ast_int_literal(const char *value, parse_state_t *ps);
ast_t *ast_id(const char *value, parse_state_t *ps);

/* Return a string corresponding to the AST type.
   Parameters:
        type - ast type for which to return the string

   Returns:
        string representing the ast type.
*/
const char *ast_type_string(enum AST_TYPE type);

#endif
