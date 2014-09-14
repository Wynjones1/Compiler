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
static bool parse_statement(token_t **tokens_, ast_t **ret , symbol_table_t *table);
static bool parse_statement_list(token_t **tokens_, ast_t **out, symbol_table_t *table);

char *copy_string(const char *string)
{
	size_t s = strlen(string);
	char *out = (char*) malloc(s + 1);
	strcpy(out, string);
	return out;
}

static identifier_t *make_id(const char *name)
{
	identifier_t *out = MALLOC_T(identifier_t);
	out->name = copy_string(name);
	out->type = AST_TYPE_ID;
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
			*import = MALLOC_T(import_t);
			(*import)->type       = AST_TYPE_IMPORT;
			(*import)->name       = make_id(tokens[1].string);
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
		*type = CALLOC_T(1, type_t);
		if(strcmp(tokens[0].string, "int") == 0)
			(*type)->is_int = true;
		else if(strcmp(tokens[0].string, "float") == 0)
			(*type)->is_float = true;

		(*type)->type = AST_TYPE_TYPE;
		(*type)->name = make_id(tokens[1].string);

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
			*decl         = MALLOC_T(decl_t);
			(*decl)->type = AST_TYPE_DECL;
			(*decl)->t    = type;
			(*decl)->name = make_id(tokens->string);
			tokens++;

			if(tokens[0].type == TOK_OP && tokens[0].op == OP_EQ)
			{
				tokens++;
				if(!parse_expression(&tokens, &(*decl)->expr, table)) return false;
			}
			*tokens_ = tokens;
			return true;
		}
		else if(!need_varname)
		{
			*decl = MALLOC_T(decl_t);
			(*decl)->type = AST_TYPE_DECL;
			(*decl)->t = type;
			(*decl)->name = NULL;
			*tokens_ = tokens;
			return true;
		}
	}
	return false;
}

static bool parse_decl_list(token_t **tokens_, decl_list_t **list, symbol_table_t *table, bool need_varname)
{
	token_t *tokens = *tokens_;
	decl_t *decl;
	*list = MALLOC_T(decl_list_t);
	(*list)->type     = AST_TYPE_DECL_LIST;
	(*list)->size     = 0;
	(*list)->decls    = NULL;
	if(parse_decl(&tokens, &decl, table, need_varname))
	{
		MALLOC_T(decl_t*);
		(*list)->decls = MALLOC_T(decl_t*);
		(*list)->decls[0] = decl;
		(*list)->size = 1;
		while(IS_COMMA(tokens[0]))
		{
			tokens++;
			if(parse_decl(&tokens, &decl, table, need_varname))
			{
				(*list)->decls = REALLOC_T((*list)->decls, decl_t*, ++((*list)->size));
				(*list)->decls[(*list)->size - 1] = decl;
			}
			else
			{
				ERROR();//TODO: Emit proper error.
			}
		}
		*tokens_ = tokens;
	}
	return true;
}

static bool parse_return(token_t **tokens_, ast_t **ret , symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	if(IS_KEYWORD(tokens[0], RETURN))
	{
		ast_t *expr;
		tokens++;
		return_t *temp = MALLOC_T(return_t);
		temp->type = AST_TYPE_RETURN;
		if(parse_expression(&tokens, &expr, table))
		{
			temp->expr = expr;
		}
		else
		{
			temp->expr = NULL;
		}
		*ret = (ast_t*)temp;
		*tokens_ = tokens;
		return true;
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
		exprs = REALLOC_T(exprs, ast_t*, ++size);
		exprs[size - 1] = expr;
		if(!IS_COMMA(tokens[0])) break;
		tokens++;
	}
	ret->size = size;
	ret->exprs = exprs;
	*tokens_ = tokens;
	return true;
}

static bool parse_function_call(token_t **tokens_, ast_t *call, function_call_t **ret , symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	if(IS_LPAREN(tokens[0])) //Function Call
	{
		param_list_t list;
		tokens++;
		if(parse_param_list(&tokens, &list, table))
		{
			if(IS_RPAREN(tokens[0]))
			{
				*ret = MALLOC_T(function_call_t);
				(*ret)->type = AST_TYPE_FUNC_CALL;
				(*ret)->call = call;
				(*ret)->params = list;
				*tokens_ = tokens + 1;
				return true;
			}
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

static bool parse_binop(token_t **tokens_, ast_t *left, binop_t **ret, symbol_table_t *table)
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
			binop_t *binop = MALLOC_T(binop_t);
			binop->type    = AST_TYPE_BINOP;
			binop->left    = left;
			binop->right   = right;
			binop->op      = op;
			if(binop->right->type == AST_TYPE_BINOP &&
				binop->op == OP_MUL && ((binop_t*)binop->right)->op == OP_ADD)
			{
				binop_t *temp = (binop_t*)binop->right;
				binop->right  = temp->left;
				temp->left    = (ast_t*)binop;
				binop = (binop_t*)temp;
			}
			*tokens_ = tokens;
			*ret     = binop;
			return true;
		}
	}
	return false;
}

#define X(NAME, field)\
	if(IS_ ## NAME(tokens[0]))\
	{\
		(*ret)= MALLOC_T(literal_t);\
		(*ret)->type = AST_TYPE_ ## NAME;\
		(*ret)->field = tokens[0].field;\
		*tokens_ = tokens + 1;\
		return true;\
	}
static bool parse_literal(token_t **tokens_, literal_t **ret , symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	X(INTEGER, integer);
	X(FLOAT, flt);
	X(STRING, string);
	return false;
}

static bool parse_id(token_t **tokens_, identifier_t **id, symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	if(IS_ID(tokens[0]))
	{
		*id = make_id(tokens->string);
		*tokens_    = tokens + 1;
		return true;
	}
	return false;
}

static bool parse_expression(token_t **tokens_, ast_t **ret , symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	ast_t  *expr;
	decl_t *decl;
	if(parse_return(&tokens, ret, table))
	{
		*tokens_ = tokens;
		return true;
	}
	if( parse_decl(&tokens, &decl, table, true))
	{
		*ret = (ast_t*)decl;
		*tokens_ = tokens;
		return true;
	}
	if( parse_paren_expression(&tokens, &expr, table)     ||
		parse_literal(&tokens, (literal_t**)&expr, table) ||
		parse_id(&tokens, (identifier_t**)&expr, table))
	{
		function_call_t *func_call;
		binop_t         *binop;
		while(parse_function_call(&tokens, expr, &func_call, table))
		{
			expr = (ast_t*) func_call;
		}
		if(parse_binop(&tokens, expr, &binop, table))
		{
			expr = (ast_t*) binop;
		}
		*ret     = expr;
		*tokens_ = tokens;
		return true;
	}
	return false;
}

static bool parse_bool(token_t **tokens_, ast_t **ret , symbol_table_t *table)
{
	//TODO:Fix
	return parse_expression(tokens_, ret, table);
}

static bool parse_conditional(token_t **tokens_, ast_t **ret , symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	if(IS_KEYWORD(tokens[0], IF) && IS_LPAREN(tokens[1]))
	{
		tokens += 2;
		ast_t *cond;
		ast_t *statement;
		if_t *temp;
		if(!parse_bool(&tokens, &cond, table)) return false;
		if(!IS_RPAREN(tokens[0])) return false;
		tokens++;
		if(IS_NEWLINE(tokens[0])) tokens++;
		if(!parse_statement(&tokens, &statement, table)) return false;

		temp            = MALLOC_T(if_t);
		temp->type      = AST_TYPE_IF;
		temp->cond      = cond;
		temp->succ      = statement;
		temp->fail      = NULL;

		*ret            = (ast_t*)temp;
		*tokens_        = tokens;
		return true;
	}
	*ret = NULL;
	return false;
}

static bool parse_statement(token_t **tokens_, ast_t **ret , symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	if(parse_expression(&tokens, ret, table))
	{
		if(tokens[0].type != TOK_SEMICOLON)
		{
			print_token(tokens);
			print_token(tokens + 1);
			ERROR();
		}
		*tokens_ = tokens + 1;
		return true;
	}
	if(parse_conditional(&tokens, ret, table))
	{
		*tokens_ = tokens;
		return true;
	}
	if(parse_statement_list(&tokens, ret, table))
	{
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
	ast_t **statements = NULL;
	ast_t *statement;
	int size = 0;
	while(IS_NEWLINE(tokens[0])) tokens++;
	while(parse_statement(&tokens, &statement, table))
	{
		while(IS_NEWLINE(tokens[0])) tokens++;
		statements = REALLOC_T(statements, ast_t*, ++size);
		statements[size - 1] = statement;
	}

	if(!IS_RBRACE(tokens[0]))
	{
		//TODO:Cleanup.
		*out = NULL;
		return false;
	}

	if(size == 1)
	{
		*out = statements[0];
		free(statements);
	}
	else
	{
		statement_list_t *list;
		list             = MALLOC_T(statement_list_t);
		list->type       = AST_TYPE_STATEMENT_LIST;
		list->size       = size;
		list->statements = statements;
		*out             = (ast_t*) list;
	}
	*tokens_ = tokens + 1;
	return true;
}

static bool parse_function(token_t **tokens_, function_t **function, symbol_table_t *table)
{
	*function = NULL;
	token_t *tokens = *tokens_;
	if( IS_KEYWORD(tokens[0], FUNCTION) &&
		IS_ID(tokens[1]) &&
		IS_LPAREN(tokens[2]))
	{
		char *name = copy_string(tokens[1].string);
		tokens += 3;
		decl_list_t *inputs, *outputs;
		if(parse_decl_list(&tokens, &inputs, table, true))
		{
			if(!IS_RPAREN(tokens[0])) ERROR();
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
			function_t *temp = MALLOC_T(function_t);
			temp->type       = AST_TYPE_FUNCTION;
			temp->input      = inputs;
			temp->output     = outputs;
			temp->statements = statements;
			temp->name       = make_id(name);
			temp->table      = NULL;
			*function = temp;
			*tokens_  = tokens;
			return true;
		}
	}
	return false;
}

program_t *parse(token_t *tokens)
{
	//Begins with a list of imports
	program_t *program = MALLOC_T(program_t);
	program->type  = AST_TYPE_PROGRAM;
	//TODO: Add symbol table
	symbol_table_t *table    = NULL;
	import_t       *import;
	function_t     *function;
	function_t     **functions = NULL;
	import_t       **imports   = NULL;
	int num_imports   = 0;
	int num_functions = 0;

	while(parse_import(&tokens, &import, table))
	{
		imports = REALLOC_T(imports, import_t*, ++num_imports);
		imports[num_imports - 1] = import;
	}

	while(IS_NEWLINE(*tokens))
	{
		tokens++;
	}

	while(parse_function(&tokens, &function, table))
	{
		functions = REALLOC_T(functions, function_t*, ++num_functions);
		functions[num_functions- 1] = function;
	}
	program->num_imports   = num_imports;
	program->imports       = imports;
	program->num_functions = num_functions;
	program->functions     = functions;
	return program;
}
