#pragma once
#include <stdbool.h>
#include "lexer.h"
#include "symbol.h"

//Forward declarations.
typedef struct ast  ast_t;
typedef struct type type_t;

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
	AST_TYPE_PROGRAM,
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

struct type
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
};

struct ast
{
	enum AST_TYPE type;
};

typedef struct return_s
{
	enum AST_TYPE type;
	ast_t        *expr;
}return_t;

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
	enum AST_TYPE type;
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

typedef struct statement_list
{
	enum AST_TYPE type;
	int size;
	ast_t **statements;
}statement_list_t;

typedef struct function
{
	enum AST_TYPE type;
	int size;
	decl_list_t      *input;
	decl_list_t      *output;
	statement_list_t *statements;
	symbol_table_t   *table;
}function_t;

typedef struct literal
{
	enum AST_TYPE type;
	union
	{
		int         integer;
		float       flt;
		const char *string;
	};
}literal_t;

typedef struct identifier
{
	enum AST_TYPE type;
	const char *name;
}identifier_t;

typedef struct program
{
	enum AST_TYPE type;
	int           num_imports;
	int           num_functions;
	import_t    **imports;
	function_t  **functions;
}program_t;

#if 0
	union
	{
		while_t           ast_while;
		if_t              ast_if;
		decl_t            decl;
		binop_t           binop;
		function_t        function;
		function_call_t   call;
		program_t         program;
		struct ast       *expr;
		statement_list_t  statements;
		const char       *string;
		int               integer;
		float             flt;
	};
#endif
