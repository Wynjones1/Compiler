#ifndef AST_H
#define AST_H
#include <stddef.h>
#include "lexer.h"
#include "operators.h"

enum AST_TYPE
{
    AST_TYPE_FUNCTION,
    AST_TYPE_TYPE_DECL,
    AST_TYPE_PARAM,
    AST_TYPE_LIST,
    AST_TYPE_RETURN,
    AST_TYPE_ID,
    AST_TYPE_INT_LIT,
    AST_TYPE_VAR_DECL,
    AST_TYPE_WHILE,
    AST_TYPE_IF,
    AST_TYPE_OPERATION,
    AST_TYPE_FUNC_CALL,
};

typedef struct ast ast_t;
typedef struct typedecl typedecl_t;

struct typedecl
{
    const char *id;
};

struct ast
{
    enum AST_TYPE type;
    union
    {
        struct
        {
            const char *name;
            ast_t      *params;
            ast_t      *return_;
            ast_t      *statements;
        }function;

        struct
        {
            ast_t  **data;
            size_t   count;
        }list;

        struct
        {
            ast_t      *type;
            const char *name;
        }param;

        struct
        {
            ast_t      *type;
            const char *name;
            ast_t      *expr;
        }vardecl;

        /* Any infix or unary operator
            When binary arg_0 and arg_1 will both contain data.
            When unary only lhs will be used.
        */
        struct
        {
            enum OPERATOR type;
            ast_t *arg_0;
            ast_t *arg_1;
        }op;

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

        const char *id;
        const char *int_literal;

        typedecl_t decl;
    };
};

/*  Create an AST list.

    Returns:
        Empty AST list.
*/
ast_t *ast_list();

/*  Create a generic AST.

    Parameters:
        type - type of the AST to create.
*/
ast_t *ast_make(enum AST_TYPE type);

/*  Append an element to the end of and AST list.
    Parameters:
        list - list to which to append the element.
        elem - element to be inserted.
*/
void ast_list_append(ast_t *list, ast_t *elem);

#endif
