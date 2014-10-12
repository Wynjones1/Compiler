#pragma once
#include <stdbool.h>
#include "lexer.h"

//Forward declarations.
typedef struct ast  ast_t;
typedef struct type type_t;
typedef struct symbol_table symbol_table_t;

enum AST_TYPE
{
	AST_TYPE_ASSIGN,
	AST_TYPE_BINOP,
	AST_TYPE_DECL,
	AST_TYPE_DECL_LIST,
	AST_TYPE_DO,
	AST_TYPE_FLOAT,
	AST_TYPE_FOR,
	AST_TYPE_FUNCTION,
	AST_TYPE_FUNC_CALL,
	AST_TYPE_ID,
	AST_TYPE_IF,
	AST_TYPE_IMPORT,
	AST_TYPE_INTEGER,
	AST_TYPE_LIST,
	AST_TYPE_NEG,
	AST_TYPE_NONE,
	AST_TYPE_PROGRAM,
	AST_TYPE_RETURN,
	AST_TYPE_STATEMENT_LIST,
	AST_TYPE_STRING,
	AST_TYPE_TYPE,
	AST_TYPE_WHILE,
	NUM_AST_TYPES
};

enum TYPE_MOD
{
	TYPE_MOD_ARRAY,
	TYPE_MOD_NONE,
	TYPE_MOD_POINTER,
	NUM_TYPE_MODS
};

struct ast
{
	enum AST_TYPE type;
};

typedef struct ast_list
{
	enum AST_TYPE type;
	int size;
	ast_t **exprs;
}ast_list_t;

typedef struct ast_list param_list_t;
typedef struct ast_list decl_list_t;
typedef struct ast_list expr_list_t;

typedef struct statement_list
{
	enum AST_TYPE type;
	int size;
	ast_t **exprs;
	symbol_table_t *table;
}statement_list_t;

typedef struct identifier
{
	enum AST_TYPE type;
	char *name;
}identifier_t;

typedef struct import
{
	enum AST_TYPE    type;
	bool             add_to_global;
	identifier_t    *name;
	identifier_t    *to_import;
}import_t;

struct type
{
	enum AST_TYPE type;
	enum TYPE_MOD mod;
	bool is_constant;
	bool is_static;
	bool is_int;
	bool is_float;
	union
	{
		identifier_t *name;
		struct type  *next;
	};
};

typedef struct return_s
{
	enum AST_TYPE type;
	ast_t        *expr;
}return_t;


typedef struct assign
{
	enum AST_TYPE type;
	identifier_t  *id;
	ast_t         *expr;
}assign_t;

typedef struct function_call
{
	enum AST_TYPE type;
	ast_t *call;
	param_list_t params;
}function_call_t;

typedef struct binop
{
	enum AST_TYPE type;
	enum OP op;
	ast_t *left;
	ast_t *right;
}binop_t;

typedef struct decl
{
	enum AST_TYPE type;
	type_t       *t;
	identifier_t *name;
	ast_t        *expr;
}decl_t;


typedef struct if_s
{
	enum AST_TYPE type;
	ast_t *cond;
	ast_t *succ;
	ast_t *fail;
}if_t;

typedef struct while_s
{
	enum AST_TYPE type;
	ast_t *cond;
	ast_t *statement;
}while_t;

typedef struct  for_s
{
	enum AST_TYPE type;
	ast_t *init;
	ast_t *cond;
	ast_t *post;
	ast_t *expr;
	symbol_table_t *table;
}for_t;

typedef struct do_s
{
	enum AST_TYPE type;
	ast_t *cond;
	ast_t *statement;
}do_t;

typedef struct function
{
	enum AST_TYPE type;
	int size;
	identifier_t     *name;
	decl_list_t      *input;
	decl_list_t      *output;
	ast_t            *statements;
	symbol_table_t   *table;
}function_t;

typedef struct literal
{
	enum AST_TYPE type;
	union
	{
		int      integer;
		float    flt;
		char    *string;
	};
}literal_t;

typedef struct neg
{
	enum AST_TYPE type;
	ast_t *expr;
}neg_t;

typedef struct program
{
	enum AST_TYPE type;
	int           num_imports;
	int           num_functions;
	import_t    **imports;
	function_t  **functions;
}program_t;

#define ast_print(ast, fp) ast_print_((ast_t*)ast, fp)
void ast_print_(ast_t *ast, FILE *fp);
#define ast_delete(ast) ast_delete_((ast_t*)ast)
void ast_delete_(ast_t *to_delete);

//TODO:Remove this and just make it an allocated list wherever it is used.
void delete_list_static(ast_list_t *l);
