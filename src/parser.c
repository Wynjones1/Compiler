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

#define ERROR()\
do{\
	print_token(tokens);\
	fflush(stdout);\
	fprintf(stderr, "Error %s %d\n", __FILE__, __LINE__);\
	exit(-1);}\
while(0);


static bool parse_expression(token_t **tokens_, ast_t **ret , symbol_table_t *table);
static bool parse_statement(token_t **tokens_, ast_t **ret , symbol_table_t *table);
static bool parse_statement_list(token_t **tokens_, ast_t **out, symbol_table_t *table);
static bool parse_assignment(token_t **tokens_, ast_t **ret , symbol_table_t *table);
static bool parse_id(token_t **tokens_, identifier_t **id, symbol_table_t *table);
static bool parse_literal(token_t **tokens_, literal_t **ret , symbol_table_t *table);
static bool parse_paren_expression(token_t **tokens_, ast_t **ret , symbol_table_t *table);

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
	if( IS_KEYWORD(tokens[0], IMPORT))
	{
		if( IS_ID(tokens[1]) &&
			IS_SEMICOLON(tokens[2]))
		{
			*import = MALLOC_T(import_t);
			(*import)->type          = AST_TYPE_IMPORT;
			(*import)->name          = make_id(tokens[1].string);
			(*import)->add_to_global = false;
			(*import)->to_import     = NULL;
			*tokens_ = tokens + 3;
			return true;
		}
		else
		{
			ERROR();
		}
	}
	else if(IS_KEYWORD(tokens[0], FROM))
	{
		if( IS_ID(tokens[1]) &&
			IS_KEYWORD(tokens[2], IMPORT) &&
			IS_ID(tokens[3]) &&
			IS_SEMICOLON(tokens[4]))
		{
			*import = MALLOC_T(import_t);
			(*import)->type          = AST_TYPE_IMPORT;
			(*import)->name          = make_id(tokens[1].string);
			(*import)->to_import     = make_id(tokens[3].string);
			(*import)->add_to_global = true;
			*tokens_ = tokens + 5;
			return true;
		}
		else
		{
			tokens++;
			print_token(tokens);
			ERROR();
		}
	}
	*import = NULL;
	return false;
}

static bool parse_type(token_t **tokens_, type_t **type, symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	bool isstatic = false;
	bool isconst  = false;
	if(IS_KEYWORD(tokens[0], STATIC))
	{
		isstatic = true;
		tokens++;
	}
	if(IS_KEYWORD(tokens[0], CONST))
	{
		isconst = true;
		tokens++;
	}
	if(IS_ID(tokens[0]))
	{
		*type = CALLOC_T(1, type_t);
		if(strcmp(tokens[0].string, "int") == 0)
			(*type)->is_int = true;
		else if(strcmp(tokens[0].string, "float") == 0)
			(*type)->is_float = true;

		(*type)->type        = AST_TYPE_TYPE;
		(*type)->name        = make_id(tokens[0].string);
		(*type)->is_static   = isstatic;
		(*type)->is_constant = isconst;

		*tokens_ = tokens + 1;
		return true;
	}
	*type = NULL;
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
			(*decl)->expr = NULL;
			tokens++;

			if(tokens[0].type == TOK_ASSIGN)
			{
				tokens++;
				if(!parse_expression(&tokens, &(*decl)->expr, table))
					ERROR()
			}
			*tokens_ = tokens;
			return true;
		}
		else if(!need_varname)
		{
			*decl = MALLOC_T(decl_t);
			(*decl)->type = AST_TYPE_DECL;
			(*decl)->t    = type;
			(*decl)->name = NULL;
			(*decl)->expr = NULL;
			*tokens_ = tokens;
			return true;
		}
		ast_delete(type);
	}
	*decl = NULL;
	return false;
}

static bool parse_decl_list(token_t **tokens_, decl_list_t **list, symbol_table_t *table, bool need_varname)
{
	token_t *tokens = *tokens_;
	decl_t *decl    = NULL;
	*list           = MALLOC_T(decl_list_t);
	(*list)->type   = AST_TYPE_DECL_LIST;
	(*list)->size   = 0;
	(*list)->exprs  = NULL;

	if(parse_decl(&tokens, &decl, table, need_varname))
	{
		(*list)->exprs    = MALLOC_T(ast_t*);
		(*list)->exprs[0] = (ast_t*) decl;
		(*list)->size     = 1;
		while(IS_COMMA(tokens[0]))
		{
			tokens++;
			if(!parse_decl(&tokens, &decl, table, need_varname))
				ERROR();
			(*list)->exprs = REALLOC_T((*list)->exprs, ast_t*, ++((*list)->size));
			(*list)->exprs[(*list)->size - 1] = (ast_t*) decl;
		}
	}
	*tokens_ = tokens;
	return true;
}

static bool parse_expression_list(token_t **tokens_, ast_t **ret , symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	if(IS_LPAREN(tokens[0]))
	{
		tokens++;
		int size      = 0;
		ast_t **exprs = NULL;
		ast_t *expr   = NULL;
		while(parse_expression(&tokens, &expr, table))
		{
			exprs = REALLOC_T(exprs, ast_t*, ++size);
			exprs[size - 1] = expr;
			if(!IS_COMMA(tokens[0])) break;
			tokens++;
		}
		if(IS_RPAREN(tokens[0]))
		{
			expr_list_t *temp = MALLOC_T(expr_list_t);
			temp->type        = AST_TYPE_LIST;
			temp->size        = size;
			temp->exprs       = exprs;
			*ret              = (ast_t*)temp;
			*tokens_          = tokens + 1;
			return true;
		}
		else
		{
			ERROR();
		}
	}
	*ret = NULL;
	return false;
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
		temp->expr = NULL;
		if( parse_expression_list(&tokens, &expr, table) ||
			parse_expression(&tokens, &expr, table))
		{
			temp->expr = expr;
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
			if(!IS_RPAREN(tokens[0]))
				ERROR();

			*ret = MALLOC_T(function_call_t);
			(*ret)->type = AST_TYPE_FUNC_CALL;
			(*ret)->call = call;
			(*ret)->params = list;
			*tokens_ = tokens + 1;
			return true;
		}
	}
	*ret = NULL;
	return false;
}

static bool parse_term(token_t **tokens_, ast_t **ret, symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	ast_t *temp;
	if(parse_id(&tokens, (identifier_t**) &temp, table)    ||
		parse_paren_expression(&tokens, &temp, table))
	{
		ast_t *call;
		while(parse_function_call(&tokens, temp, (function_call_t**) &call, table))
		{
			temp = call;
		}
		*tokens_ = tokens;
		*ret = temp;
		return true;
	}
	else if(parse_literal(&tokens, (literal_t**) ret, table))
	{
		*tokens_ = tokens;
		return true;
	}
	else if(IS_OP(tokens[0], SUB))
	{
		tokens++;
		ast_t *temp;
		if(parse_term(&tokens, &temp, table))
		{
			neg_t *n = MALLOC_T(neg_t);
			n->type = AST_TYPE_NEG;
			n->expr = temp;
			*tokens_ = tokens;
			*ret = (ast_t*)n;
			return true;
		}
	}
	*ret = NULL;
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
			if(!IS_RPAREN(tokens[0]))
				ERROR();
			*tokens_ = tokens + 1;
			return true;
		}
	}
	*ret = NULL;
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
	if(IS_STRING(tokens[0]))
	{
		(*ret)= MALLOC_T(literal_t);
		(*ret)->type = AST_TYPE_STRING;
		(*ret)->string = copy_string(tokens[0].string);
		*tokens_ = tokens + 1;
		return true;
	}
	*ret = NULL;
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
	*id = NULL;
	return false;
}

#if 0
static binop_t *make_binop(enum OP op, ast_t *left, ast_t *right)
{
	binop_t *out = MALLOC_T(binop_t);
	out->type    = AST_TYPE_BINOP;
	out->op      = op;
	out->left    = left;
	out->right   = right;
	return out;
}

static int comp_precedence(enum OP op0, enum OP op1)
{
	if(op_precedence(op0) == op_precedence(op1)) return 0;
	if(op_precedence(op0) <  op_precedence(op1))  return -1;
	return 1;
}
#endif

/*
 * Parses binary operations with terms inbetween, parenthesised expressions are
 * dealt with in parse_term so we only have to care about terms and operations.
 */
static bool shunting_yard(token_t **tokens_, ast_t *first, ast_t **out, symbol_table_t *table)
{
	static const int STACK_SIZE = 100;
	token_t *tokens = *tokens_;
	ast_t *operand_stack[STACK_SIZE];
	ast_t *term;
	binop_t *new;
	enum OP operator_stack[STACK_SIZE];
	enum OP op1, op2;
	int operand_size  = 1;
	int operator_size = 0;
	operand_stack[0] = first;

	while(true)
	{
		if(tokens[0].type == TOK_OP)
		{
			op1 = tokens[0].op;
			while(operator_size)
			{
				op2 = operator_stack[operator_size - 1];
				if((op_associative(op1) ==  ASSOC_LEFT          &&
					op_precedence(op1) >= op_precedence(op2)) ||
				   (op_precedence(op1) >  op_precedence(op2)))
				{
					if(operand_size < 2)
						ERROR();
					new         = MALLOC_T(binop_t);
					new->type   = AST_TYPE_BINOP;
					new->right  = operand_stack[--operand_size];
					new->left   = operand_stack[--operand_size];
					new->op     = op2;
					operand_stack[operand_size++] = (ast_t*)new;
					operator_size--;
				}
				else break;
			}
			operator_stack[operator_size++] = op1;
			tokens++;

			if(!parse_term(&tokens, &term, table)) ERROR();
			operand_stack[operand_size++] = term;
		}
		else
		{
			while(true)
			{
				if(operand_size == 1 && operator_size == 0)
				{
					*out = operand_stack[0];
					*tokens_ = tokens;
					return true;
				}
				else if(operand_size > 1 && operator_size > 0)
				{
					new         = MALLOC_T(binop_t);
					new->type   = AST_TYPE_BINOP;
					new->right  = operand_stack[--operand_size];
					new->left   = operand_stack[--operand_size];
					new->op     = operator_stack[--operator_size];
					operand_stack[operand_size++] = (ast_t*)new;
				}
				else
					ERROR();
			}
		}
	}
}

static bool parse_binop(token_t **tokens_, ast_t **ret, symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	ast_t *left;
	if(parse_term(&tokens, &left, table))
	{
		if(tokens->type == TOK_OP)
		{
			if(!shunting_yard(&tokens, left, ret, table))
				ERROR();
		}
		else
		{
			*ret = left;
		}
		*tokens_ = tokens;
		return true;
	}
	*ret = NULL;
	return false;
}

static bool parse_expression(token_t **tokens_, ast_t **ret , symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	ast_t  *expr;
	if( parse_assignment(&tokens, &expr, table))
	{
		ERROR();
	}
	if( parse_binop(&tokens, &expr, table))
	{
		*ret     = expr;
		*tokens_ = tokens;
		return true;
	}
	*ret = NULL;
	return false;
}

static bool parse_bool(token_t **tokens_, ast_t **ret , symbol_table_t *table)
{
	//TODO:Fix
	return parse_expression(tokens_, ret, table);
}

static bool parse_else(token_t **tokens_, ast_t **ret , symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	if(IS_KEYWORD(tokens[0], ELSE))
	{
		tokens += 1;
		if(IS_NEWLINE(tokens[0])) tokens++;
		if(!parse_statement(&tokens, ret, table))
			ERROR();

		*tokens_ = tokens;
		return true;
	}
	*ret = NULL;
	return false;
}

static bool parse_if(token_t **tokens_, ast_t **ret , symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	if(IS_KEYWORD(tokens[0], IF))
	{
		if(!IS_LPAREN(tokens[1]))
		{
			//TODO:IF Warning.
			ERROR();
		}
		tokens += 2;
		ast_t *cond;
		ast_t *statement;
		if_t *temp;
		if(!parse_bool(&tokens, &cond, table))
			ERROR();
		if(!IS_RPAREN(tokens[0]))
			ERROR();
		tokens++;
		if(IS_NEWLINE(tokens[0])) tokens++;
		if(!parse_statement(&tokens, &statement, table))
			ERROR();

		temp            = MALLOC_T(if_t);
		temp->type      = AST_TYPE_IF;
		temp->cond      = cond;
		temp->succ      = statement;
		while(IS_NEWLINE(tokens[0]))tokens++;
		parse_else(&tokens, &temp->fail, table);

		*ret            = (ast_t*)temp;
		*tokens_        = tokens;
		return true;
	}
	*ret = NULL;
	return false;
}

static bool parse_while(token_t **tokens_, ast_t **ret , symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	if(IS_KEYWORD(tokens[0], WHILE))
	{
		if(!IS_LPAREN(tokens[1]))
			ERROR();
		tokens += 2;
		ast_t *cond;
		ast_t *statement;
		while_t *temp;
		if(!parse_bool(&tokens, &cond, table))
			ERROR();
		if(!IS_RPAREN(tokens[0]))
			ERROR();
		tokens++;
		if(IS_NEWLINE(tokens[0])) tokens++;
		if(!parse_statement(&tokens, &statement, table))
			ERROR();

		temp            = MALLOC_T(while_t);
		temp->type      = AST_TYPE_WHILE;
		temp->cond      = cond;
		temp->statement = statement;
		*ret            = (ast_t*)temp;
		*tokens_        = tokens;
		return true;
	}
	*ret = NULL;
	return false;
}
static bool parse_do(token_t **tokens_, ast_t **ret , symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	ast_t *cond;
	ast_t *statement;
	do_t *temp;
	if(IS_KEYWORD(tokens[0], DO))
	{
		tokens++;
		if(IS_NEWLINE(tokens[0])) tokens++;
		if(!parse_statement(&tokens, &statement, table))
			ERROR();
		if(IS_NEWLINE(tokens[0])) tokens++;
		if(!IS_KEYWORD(tokens[0], WHILE))
			ERROR();
		if(!IS_LPAREN(tokens[1]))
			ERROR();
		tokens += 2;
		if(!parse_bool(&tokens, &cond, table)) 
			ERROR();
		if(!IS_RPAREN(tokens[0])) 
			ERROR();
		if(!IS_SEMICOLON(tokens[1])) 
			ERROR();
		tokens += 2;

		temp            = MALLOC_T(do_t);
		temp->type      = AST_TYPE_DO;
		temp->cond      = cond;
		temp->statement = statement;
		*ret            = (ast_t*)temp;
		*tokens_        = tokens;
		return true;
	}
	*ret = NULL;
	return false;
}

static bool parse_for(token_t **tokens_, ast_t **ret , symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	if(IS_KEYWORD(tokens[0], FOR))
	{
		for_t *out = MALLOC_T(for_t);
		out->type = AST_TYPE_FOR;
		ast_t *init, *cond, *post, *expr;
		if(!IS_LPAREN(tokens[1])) 
			ERROR();
		tokens += 2;
		if(!(parse_decl(&tokens, (decl_t**)&init, table, true) ||
			 parse_assignment(&tokens, &init, table)           ||
			 parse_expression(&tokens, &init, table) )) 
			 	ERROR();
		if(!IS_SEMICOLON(tokens[0])) 
			ERROR();
		tokens++;
		if(!parse_expression(&tokens, &cond, table)) 
			ERROR();
		if(!IS_SEMICOLON(tokens[0])) 
			ERROR();
		tokens++;
		if(!parse_expression(&tokens, &post, table)) 
			ERROR();
		if(!IS_RPAREN(tokens[0])) 
			ERROR();
		tokens++;
		if(IS_NEWLINE(tokens[0])) tokens++;
		if(!parse_statement(&tokens, &expr, table)) 
			ERROR();
		out->init = init;
		out->cond = cond;
		out->post = post;
		out->expr = expr;
		*tokens_  = tokens;
		*ret      = (ast_t*)out;
		return true;
	}
	*ret = NULL;
	return false;
}

static bool parse_conditional(token_t **tokens_, ast_t **ret , symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	if(parse_if(&tokens, ret, table)    ||
	   parse_while(&tokens, ret, table) ||
	   parse_for(&tokens, ret, table)   ||
	   parse_do(&tokens, ret, table))
	{
		*tokens_ = tokens;
		return true;
	}
	*ret = NULL;
	return false;
}

static bool parse_assignment(token_t **tokens_, ast_t **ret , symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	if(IS_ID(tokens[0]) && tokens[1].type == TOK_ASSIGN)
	{
		ast_t *expr;
		assign_t *assign;
		const char *name = tokens->string;
		tokens += 2;
		if(!parse_expression(&tokens, &expr, table))
			ERROR();
		assign       = MALLOC_T(assign_t);
		assign->type = AST_TYPE_ASSIGN;
		assign->id   = make_id(name);
		assign->expr = expr;
		*ret         = (ast_t*)assign;
		*tokens_     = tokens;
		return true;
	}
	*ret = NULL;
	return false;
}

static bool parse_statement(token_t **tokens_, ast_t **ret , symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	if( parse_return(&tokens, ret, table)               ||
		parse_decl(&tokens, (decl_t**)ret, table, true) ||
		parse_assignment(&tokens, ret, table)           ||
		parse_expression(&tokens, ret, table))
	{
		if(tokens[0].type != TOK_SEMICOLON)
		{
			ERROR();
		}
		*tokens_ = tokens + 1;
		return true;
	}

	if( parse_conditional(&tokens, ret, table)    ||
		parse_statement_list(&tokens, ret, table))
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
		if(IS_RBRACE(tokens[0]))
		{
			break;
		}
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
		list->exprs      = statements;
		*out             = (ast_t*) list;
	}
	*tokens_ = tokens + 1;
	return true;
}

static bool parse_function_proto(token_t **tokens_, function_t **function, symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	if( IS_KEYWORD(tokens[0], FUNCTION) &&
		IS_ID(tokens[1]) &&
		IS_LPAREN(tokens[2]))
	{
		char *name = tokens[1].string;
		tokens += 3;
		decl_list_t *inputs = NULL, *outputs = NULL;
		if(parse_decl_list(&tokens, &inputs, table, true))
		{
			if(!IS_RPAREN(tokens[0]))
				ERROR();
			tokens++;
			if(IS_ARROW(tokens[0]))
			{
				tokens++;
				if(IS_LPAREN(tokens[0]))
				{
					tokens++;
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
				else if(IS_ID(tokens[0]))
				{
					if(!parse_decl_list(&tokens, &outputs, table, false))
						ERROR();
				}
				else
					ERROR();
			}
			else
			{
				outputs        = MALLOC_T(decl_list_t);
				outputs->type  = AST_TYPE_DECL_LIST;
				outputs->size  = 0;
				outputs->exprs = NULL;
			}
			function_t *temp = MALLOC_T(function_t);
			temp->type       = AST_TYPE_FUNCTION;
			temp->input      = inputs;
			temp->output     = outputs;
			temp->name       = make_id(name);
			temp->table      = NULL;
			*function = temp;
			*tokens_  = tokens;
			return true;
		}
	}
	*function = NULL;
	return false;
}

static bool parse_function(token_t **tokens_, function_t **function, symbol_table_t *table)
{
	token_t *tokens = *tokens_;
	if(parse_function_proto(&tokens, function, table))
	{
		if(IS_NEWLINE(tokens[0])) tokens++;
		if(!parse_statement_list(&tokens, &(*function)->statements, table))
			ERROR();
		*tokens_  = tokens;
		return true;
	}
	*function = NULL;
	return false;
}

program_t *parse(token_t *tokens)
{
	//Begins with a list of imports
	program_t *program = MALLOC_T(program_t);
	program->type      = AST_TYPE_PROGRAM;
	//TODO: Add symbol table
	symbol_table_t *table      = NULL;
	import_t       *import     = NULL;
	function_t     *function   = NULL;
	function_t     **functions = NULL;
	import_t       **imports   = NULL;
	int num_imports   = 0;
	int num_functions = 0;

	while(IS_NEWLINE(*tokens))
	{
		tokens++;
	}

	while(parse_import(&tokens, &import, table))
	{
		while(IS_NEWLINE(tokens[0])) tokens++;
		imports = REALLOC_T(imports, import_t*, ++num_imports);
		imports[num_imports - 1] = import;
	}

	while(parse_function(&tokens, &function, table))
	{
		while(IS_NEWLINE(tokens[0])) tokens++;
		functions = REALLOC_T(functions, function_t*, ++num_functions);
		functions[num_functions - 1] = function;
	}
	if(tokens[0].type != TOK_NONE)
		ERROR();
	program->num_imports   = num_imports;
	program->imports       = imports;
	program->num_functions = num_functions;
	program->functions     = functions;
	return program;
}
