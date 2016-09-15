#ifndef AST_H
#define AST_H
#include <stddef.h>
#include "allocator.h"
#include "lexer.h"
#include "operators.h"

#define X_AST_TYPE_LIST \
    X(FUNCTION)         \
    X(TYPE_DECL)        \
    X(PARAM)            \
    X(LIST)             \
    X(RETURN)           \
    X(ID)               \
    X(INT_LIT)          \
    X(VAR_DECL)         \
    X(WHILE)            \
    X(IF)               \
    X(OPERATION)        \
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
        type - Type of the AST to create.
        al   - Allocator from which to allocate.
               If al is NULL then malloc will be used.
*/

ast_t *ast_make(enum AST_TYPE type, allocator_t *al);

/*  Append an element to the end of and AST list.
    Parameters:
        list - list to which to append the element.
        elem - element to be inserted.
*/
void ast_list_append(ast_t *list, ast_t *elem);

/* Return a string corresponding to the AST type.
   Parameters:
        type - ast type for which to return the string

   Returns:
        string representing the ast type.
*/
const char *const ast_type_string(enum AST_TYPE type);

#endif
