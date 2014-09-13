#include "parser.h"
#include "symbol.h"
#include <stdlib.h>
#include <string.h>


#define MAKE_IS(name) IS_ ## name(token) ((token).type == TOK_ ##name)
#define IS_KEYWORD(token, k) ((token).type == TOK_KEYWORD && (token).kw == KW_ ## k)
#define IS_OP(token, k)      ((token).type == TOK_OP && (token).op == OP_ ## k)
#define IS_ID(token)         ((token).type == TOK_ID)
#define IS_NEWLINE(token)    ((token).type == TOK_NEWLINE)
#define IS_SEMICOLON(token)  ((token).type == TOK_SEMICOLON)
#define IS_LPAREN(token)     ((token).type == TOK_LPAREN)
#define IS_RPAREN(token)     ((token).type == TOK_RPAREN)
#define IS_LBRACE(token)     ((token).type == TOK_LBRACE)
#define IS_RBRACE(token)     ((token).type == TOK_RBRACE)
#define IS_COMMA(token)      ((token).type == TOK_COMMA)
#define IS_ARROW(token)      ((token).type == TOK_ARROW)
#define IS_INTEGER(token)    ((token).type == TOK_INTEGER)
#define IS_FLOAT(token)      ((token).type == TOK_FLOAT)
#define IS_STRING(token)     ((token).type == TOK_STRING)
#define IS_LITERAL(token)    ((token).type == TOK_INT   ||\
						      (token).type == TOK_FLOAT ||\
						      (token).type == TOK_STRING)

#define ERROR() do{fflush(stdout); fprintf(stderr, "Error %s %d\n", __FILE__, __LINE__); exit(-1);}while(0);


static bool parse_expression(token_t **tokens_, ast_t **ret , symbol_table_t *table);

static ast_t *new_ast(void)
{
	ast_t *out = malloc(sizeof(ast_t));
	out->type = AST_TYPE_NONE;
	return out;
} 
char *copy_string(const char *string)
{
	size_t s = strlen(string);
	char *out = malloc(s + 1);
	strcpy(out, string);
	return out;
}

static bool parse_import(token_t **tokens_, import_t **import, symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	*import = NULL;
	if( IS_KEYWORD(tokens[0], IMPORT) &&
		IS_ID(tokens[1]) &&
		IS_SEMICOLON(tokens[2]))
	{
			*import = malloc(sizeof(import_t));
			(*import)->name = copy_string(tokens[1].string);
			*tokens_ = tokens + 3;
			return true;
	}
	return false;
}

static bool parse_type(token_t **tokens_, type_t **type, symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	if(IS_ID(tokens[0]))
	{
		*type = calloc(1, sizeof(type_t));
		if(strcmp(tokens[0].string, "int") == 0)
			(*type)->is_int = true;
		else if(strcmp(tokens[0].string, "float") == 0)
			(*type)->is_float = true;
		*tokens_ = tokens + 1;
		return true;
	}
	return false;
}

static bool parse_decl(token_t **tokens_, decl_t **decl, symbol_table_t *table, bool need_varname)
{
	token_t *tokens = *tokens_;
	type_t *type;
	if(parse_type(&tokens, &type, table))
	{
		if(IS_ID(tokens[0]))
		{
			*decl = malloc(sizeof(decl_t));
			(*decl)->type = type;
			(*decl)->name = copy_string(tokens[0].string);
			*tokens_ = tokens + 1;
			return true;
		}
		else if(!need_varname)
		{
			*decl = malloc(sizeof(decl_t));
			(*decl)->type = type;
			*tokens_ = tokens;
			return true;
		}
		else
		{
			ERROR(); //Emit more descriptive error.
		}
	}
	return false;
}

static bool parse_decl_list(token_t **tokens_, decl_list_t **list, symbol_table_t *table, bool need_varname)
{
	token_t *tokens = *tokens_;
	decl_t *decl;
	if(parse_decl(&tokens, &decl, table, need_varname))
	{
		*list = malloc(sizeof(decl_list_t));
		(*list)->size  = 1;
		(*list)->decls = malloc(sizeof(decl_t*));
		(*list)->decls[0] = decl;
		while(IS_COMMA(tokens[0]))
		{
			tokens++;
			if(parse_decl(&tokens, &decl, table, need_varname))
			{
				(*list)->size  += 1;
				(*list)->decls = realloc((*list)->decls, sizeof(decl_t*) * (*list)->size);
				(*list)->decls[(*list)->size - 1] = decl;
			}
			else
			{
				ERROR();//TODO: Emit proper error.
			}
		}
		*tokens_ = tokens;
		return true;
	}
	return false;
}

static bool parse_return(token_t **tokens_, ast_t **ret , symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	if(IS_KEYWORD(tokens[0], RETURN))
	{
		ast_t *expr;
		tokens++;
		if(parse_expression(&tokens, &expr, table))
		{
			*ret = malloc(sizeof(ast_t));
			(*ret)->type = AST_TYPE_RETURN;
			(*ret)->expr = expr;
			*tokens_ = tokens;
			return true;
		}
	}
	*ret = NULL;
	return false;
}

static bool parse_param_list(token_t **tokens_, param_list_t *ret , symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	int size = 0;
	ast_t **exprs = NULL;
	ast_t *expr;
	while(parse_expression(&tokens, &expr, table))
	{
		exprs = realloc(exprs, ++size * sizeof(ast_t*));
		exprs[size - 1] = expr;
		if(!IS_COMMA(tokens[0])) break;
		tokens++;
	}
	ret->size = size;
	ret->exprs = exprs;
	*tokens_ = tokens;
	return true;
}

static bool parse_function_call(token_t **tokens_, ast_t **ret , symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	if(IS_LPAREN(tokens[0])) //Function Call
	{
		param_list_t list;
		if(parse_param_list(&tokens, &list, table))
		{

		}
	}
	return false;
}

static bool parse_paren_expression(token_t **tokens_, ast_t **ret , symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	if(IS_LPAREN(tokens[0]))
	{
		tokens++;
		if(parse_expression(&tokens, ret, table))
		{
			tokens++;
			if(IS_RPAREN(tokens[0]))
			{
				*tokens_ = tokens + 1;
				return true;
			}
			//TODO:Cleanup.
		}
	}
	return false;
}

static bool parse_binop(token_t **tokens_, ast_t *left, ast_t **ret, symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	if(tokens->type == TOK_OP) //Binop
	{
		enum OP op = tokens->op;
		tokens++;
		ast_t *right;
		if(parse_expression(&tokens, &right, table))
		{
			//TODO:Sort out the precidence rules.
			//TODO:Convert to comparison if needed.
			ast_t *binop = malloc(sizeof(ast_t));
			binop->type = AST_TYPE_BINOP;
			binop->binop.left  = left;
			binop->binop.right = right;
			binop->binop.op    = op;
			*tokens_ = tokens;
			*ret = binop;
			return true;
		}
	}
	return false;
}

#define X(NAME, field)\
	if(IS_ ## NAME(tokens[0]))\
	{\
		(*ret)= malloc(sizeof(ast_t));\
		(*ret)->type = AST_TYPE_ ## NAME;\
		(*ret)->field = tokens[0].field;\
		*tokens_ = tokens + 1;\
		return true;\
	}
static bool parse_literal(token_t **tokens_, ast_t **ret , symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	X(INTEGER, integer);
	X(FLOAT, flt);
	X(STRING, string);
	return false;
}

static bool parse_id(token_t **tokens_, ast_t **id, symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	if(IS_ID(tokens[0]))
	{
		*id = malloc(sizeof(ast_t));
		(*id)->type = AST_TYPE_ID;
		(*id)->string = copy_string(tokens->string);
		*tokens_ = tokens + 1;
		return true;
	}
	return false;
}

static bool parse_expression(token_t **tokens_, ast_t **ret , symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	ast_t *expr;
	if(parse_return(&tokens, ret, table))
	{
		*tokens_ = tokens;
		return true;
	}
	if( parse_paren_expression(&tokens, &expr, table) ||
		parse_literal(&tokens, &expr, table)          ||
		parse_id(&tokens, &expr, table))
	{
		ast_t *func_call, *binop;
		while(parse_function_call(&tokens, &func_call, table))
		{
			func_call->call.call = expr;
			expr = func_call;
		}
		if(parse_binop(&tokens, expr, &binop, table))
		{
			*ret = binop;
		}
		*ret     = expr;
		*tokens_ = tokens;
		return true;
	}
	return false;
}

static bool parse_statement(token_t **tokens_, ast_t **ret , symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	if(parse_expression(&tokens, ret, table))
	{
		print_token(tokens);
		if(tokens[0].type != TOK_SEMICOLON) ERROR();
		*tokens_ = tokens;
		return true;
	}
	*ret = NULL;
	return false;
}

static bool parse_statement_list(token_t **tokens_, ast_t **out, symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	if(!IS_LBRACE(tokens[0]))
	{
		*out= NULL;
		return false;
	}
	tokens++;
	ast_t **statements;
	ast_t *temp;
	int size = 0;
	while(IS_NEWLINE(tokens[0])) tokens++;
	while(parse_statement(&tokens, &temp, table))
	{
		while(IS_NEWLINE(tokens[0])) tokens++;
		statements = realloc(statements, ++size * sizeof(ast_t*));
		statements[size - 1] = temp;
	}

	if(!IS_RBRACE(tokens[0]))
	{
		//TODO:Cleanup.
		*out = NULL;
		return false;
	}

	temp->type = AST_TYPE_STATEMENT_LIST;
	temp->statements.size = size;
	temp->statements.statements = statements;

	*out     = temp;
	*tokens_ = tokens;
	return true;
}

static bool parse_function(token_t **tokens_, ast_t **function, symbol_table_t *table)
{
	*function = NULL;
	token_t *tokens = *tokens_;
	if( IS_KEYWORD(tokens[0], FUNCTION) &&
		IS_ID(tokens[1]) &&
		IS_LPAREN(tokens[2]))
	{
		const char *name = tokens[1].string;
		tokens += 3;
		decl_list_t *inputs, *outputs;
		if(parse_decl_list(&tokens, &inputs, table, true))
		{
			if(IS_RPAREN(tokens[0]))
			{
				tokens++;
				if(IS_ARROW(tokens[0]) && IS_LPAREN(tokens[1]))
				{
					tokens += 2;
					if(!parse_decl_list(&tokens, &outputs, table, false))
					{
						ERROR();
					}
					if(!IS_RPAREN(tokens[0]))
					{
						ERROR();
					}
					tokens += 1;
				}
				if(IS_NEWLINE(tokens[0])) tokens++;
				ast_t *statements;
				if(!parse_statement_list(&tokens, &statements, table)) ERROR();
			}
			else
			{
				ERROR();
			}
		}
		print_token(tokens);
	}
	return false;
}

ast_t *parse(token_t *tokens)
{
	//Begins with a list of imports
	ast_t *ast = malloc(sizeof(ast_t));
	//TODO: Add symbol table
	symbol_table_t *table = NULL;
	import_t   *imports;
	ast_t      *function;
	while(parse_import(&tokens, &imports, table))
	{
		//Add imports to symbol table.
	}

	while(IS_NEWLINE(*tokens))
	{
		tokens++;
	}

	while(parse_function(&tokens, &function, table))
	{
	}
	return ast;
}
