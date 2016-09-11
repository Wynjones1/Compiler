#ifndef AST_H
#define AST_H
#include <stddef.h>

enum AST_TYPE
{
    AST_TYPE_FUNCTION,
    AST_TYPE_LIST,
};

typedef struct ast ast_t;

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
    };
};

/*  Create an AST list.

    Returns:
        Empty AST list.
*/
ast_t *ast_list();

/*  Append an element to the end of and AST list.
    Parameters:
        list - list to which to append the element.
        elem - element to be inserted.
*/
void ast_list_append(ast_t *list, ast_t *elem);

#endif
