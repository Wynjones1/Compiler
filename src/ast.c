#include "ast.h"
#include <stdlib.h>

static int depth = -1;
static int counter = 0;

void print_depth(FILE *fp)
{
	for(int i = 0; i < depth; i++) fprintf(fp, "\t");
}

#define PRINT(format, ...)\
	do\
	{\
		fprintf(fp, "\"(%d) " format "\"", counter++, ##__VA_ARGS__);\
		if(fp == stdout) fprintf(fp, "\n");\
		else fprintf(fp, ",");\
	}\
	while(0);

static void print_NONE(ast_t *ast, FILE *fp)
{
	PRINT("NONE");
	exit(-1);
}

static void print_ID(ast_t *ast, FILE *fp)
{
	identifier_t *id = (identifier_t*)ast;
	PRINT("ID %s", id->name);
}

static void print_FUNCTION(ast_t *ast, FILE *fp)
{
	function_t *function = (function_t*)ast;
	PRINT("FUNCTION");
	ast_print(function->name, fp);
	if(function->input)
		ast_print(function->input, fp);
	if(function->output)
		ast_print(function->output, fp);
	ast_print(function->statements, fp);
}

static void print_FUNC_CALL(ast_t *ast, FILE *fp)
{
	function_call_t *func = (function_call_t*)ast;
	PRINT("FUNC CALL");
	ast_print(func->call, fp);
	for(int i = 0; i < func->params.size; i++)
	{
		ast_print(func->params.exprs[i], fp);
	}
}

static void print_DECL(ast_t *ast, FILE *fp)
{
	decl_t *decl = (decl_t*)ast;
	if(decl->name)
	{
		PRINT("DECL %s", decl->name->name);
		if(decl->expr)
		{
			ast_print(decl->t, fp);
			ast_print(decl->expr, fp);
		}
	}
	else
	{
		PRINT("DECL");
		ast_print(decl->t, fp);
	}
}

static void print_DECL_LIST(ast_t *ast, FILE *fp)
{
	decl_list_t *list = (decl_list_t*)ast;
	PRINT("DECL LIST");
	for(int i = 0; i < list->size; i++)
	{
		ast_print(list->exprs[i], fp);
	}
}

static void print_BINOP(ast_t *ast, FILE *fp)
{
	binop_t *binop = (binop_t*) ast;
#if 0
	PRINT("BINOP %c", (char)binop->op);
#else
	PRINT("BINOP ");
#endif
	ast_print(binop->left, fp);
	ast_print(binop->right, fp);
}

static void print_TYPE(ast_t *ast, FILE *fp)
{
	type_t *type = (type_t*)ast;
	PRINT("TYPE %s", ((identifier_t*)type->name)->name);
}

static void print_INTEGER(ast_t *ast, FILE *fp)
{
	literal_t *literal = (literal_t*)ast;
	PRINT("INT %d", literal->integer);
}

static void print_FLOAT(ast_t *ast, FILE *fp)
{
	literal_t *literal = (literal_t*)ast;
	PRINT("FLOAT %f", literal->flt);
	
}

static void print_STRING(ast_t *ast, FILE *fp)
{
	literal_t *literal = (literal_t*)ast;
	PRINT("STRING %s", literal->string);
}

static void print_IMPORT(ast_t *ast, FILE *fp)
{
	import_t *import = (import_t*)ast;
	//TODO:Parse full name.
	if(!import->add_to_global)
	{
		PRINT("IMPORT %s", ((identifier_t*)import->name)->name);
	}
	else
	{
		PRINT("IMPORT %s %s",
					((identifier_t*)import->name)->name,
					((identifier_t*)import->to_import)->name);
	}
}

static void print_STATEMENT_LIST(ast_t *ast, FILE *fp)
{
	statement_list_t *list = (statement_list_t*)ast;
	PRINT("STATEMENT LIST");
	for(int i = 0; i < list->size; i++)
	{
		ast_print(list->exprs[i], fp);
	}
}

static void print_RETURN(ast_t *ast, FILE *fp)
{
	return_t *ret = (return_t*)ast;
	PRINT("RETURN");
	if(ret->expr)
	{
		ast_print(ret->expr, fp);
	}
}

static void print_IF(ast_t *ast, FILE *fp)
{
	if_t *temp = (if_t*) ast;
	PRINT("IF");
	ast_print(temp->cond, fp);
	ast_print(temp->succ, fp);
	if(temp->fail)
	{
		ast_print(temp->fail, fp);
	}
}

static void print_PROGRAM(ast_t *ast, FILE *fp)
{
	program_t *program = (program_t*)ast;
	counter = 0;
	PRINT("PROGRAM");
	for(int i = 0; i < program->num_imports; i++)
	{
		ast_print(program->imports[i], fp);
	}
	for(int i = 0; i < program->num_functions; i++)
	{
		ast_print(program->functions[i], fp);
	}
}

static void print_ASSIGN(ast_t *ast, FILE *fp)
{
	assign_t *assign = (assign_t*)ast;
	PRINT("ASSIGN");
	ast_print((ast_t*)assign->id, fp);
	ast_print(assign->expr, fp);
}

static void print_LIST(ast_t *ast, FILE *fp)
{
	ast_list_t *list = (ast_list_t *)ast;
	PRINT("LIST %d", list->size);
	for(int i = 0; i < list->size; i++)
	{
		ast_print(list->exprs[i], fp);
	}
}

static void print_WHILE(ast_t *ast, FILE *fp)
{
	while_t *w = (while_t*)ast;
	PRINT("WHILE");
	ast_print(w->cond, fp);
	ast_print(w->statement, fp);
}

static void print_DO(ast_t *ast, FILE *fp)
{
	do_t*w = (do_t*)ast;
	PRINT("DO");
	ast_print(w->cond, fp);
	ast_print(w->statement, fp);
}

static void print_FOR(ast_t *ast, FILE *fp)
{
	for_t *f = (for_t*)ast;
	PRINT("FOR");
	ast_print(f->init,fp);
	ast_print(f->cond,fp);
	ast_print(f->post,fp);
	ast_print(f->expr,fp);
}

#define X(NAME)\
	case AST_TYPE_ ## NAME:\
		print_ ## NAME(ast, fp);\
		break
void ast_print_(ast_t *ast, FILE *fp)
{
	depth++;
	if(fp == stdout)
		print_depth(fp);
	else
		fprintf(fp, "[");

	switch(ast->type)
	{
		X(ASSIGN);
		X(BINOP);
		X(DECL);
		X(DECL_LIST);
		X(DO);
		X(FLOAT);
		X(FOR);
		X(FUNCTION);
		X(FUNC_CALL);
		X(ID);
		X(IF);
		X(IMPORT);
		X(INTEGER);
		X(LIST);
		X(NONE);
		X(PROGRAM);
		X(RETURN);
		X(STATEMENT_LIST);
		X(STRING);
		X(TYPE);
		X(WHILE);
		default:
			fprintf(stderr, "Unkown AST type %d\n", ast->type);
			exit(-1);
			break;
	}
	if(fp != stdout)
		fprintf(fp, "],");
	depth--;
}
#undef X
