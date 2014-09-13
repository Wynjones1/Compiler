#pragma once
#include <stdbool.h>
#include "lexer.h"

//Forward declarations.
typedef struct ast  ast_t;
typedef struct type type_t;
typedef struct symbol_table symbol_table_t;

enum AST_TYPE
{
	AST_TYPE_ID,
	AST_TYPE_FUNCTION,
	AST_TYPE_BINOP,
	AST_TYPE_NONE,
	AST_TYPE_TYPE,
	AST_TYPE_INTEGER,
	AST_TYPE_FLOAT,
	AST_TYPE_STRING,
	AST_TYPE_IMPORT,
	AST_TYPE_STATEMENT_LIST,
	AST_TYPE_RETURN,
	NUM_AST_TYPES
};

enum TYPE_MOD
{
	TYPE_MOD_NONE,
	TYPE_MOD_POINTER,
	TYPE_MOD_ARRAY
};

typedef struct import
{
	const char *name;
}import_t;

typedef struct type
{
	enum TYPE_MOD mod;
	bool is_constant;
	bool is_static;
	bool is_int;
	bool is_float;
	union
	{
		const char *name;
		struct type *next;
	};
}type_t;

typedef struct param_list
{
	int size;
	ast_t **exprs;
}param_list_t;

typedef struct function_call
{
	ast_t *call;
	param_list_t params;
}function_call_t;

typedef struct binop
{
	enum OP op;
	ast_t *left;
	ast_t *right;
}binop_t;

typedef struct decl
{
	type_t     *type;
	const char *name;
}decl_t;

typedef struct decl_list
{
	int size;
	decl_t **decls;
}decl_list_t;

typedef struct if_s
{
	ast_t *cond;
	ast_t *succ;
	ast_t *fail;
}if_t;

typedef struct while_s
{
	ast_t *invariant;
	ast_t *statement;
}while_t;

typedef struct function
{
	int size;
	decl_list_t    *input;
	decl_list_t    *output;
	ast_t          *statements;
	symbol_table_t *table;
}function_t;

typedef struct id
{
	const char *name;
}id_t;

typedef struct statement_list
{
	int size;
	ast_t **statements;
}statement_list_t;

typedef struct ast
{
	enum AST_TYPE type;
	union
	{
		while_t           ast_while;
		if_t              ast_if;
		decl_t            decl;
		binop_t           binop;
		function_t        function;
		function_call_t   call;
		struct ast       *expr;
		statement_list_t  statements;
		const char       *string;
		int               integer;
		float             flt;
	};
}ast_t;
