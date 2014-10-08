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
	ast_print(function->input, fp);
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
	PRINT("%s", get_binop_string(binop->op));
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
	ast_print(ret->expr, fp);
}

static void print_IF(ast_t *ast, FILE *fp)
{
	if_t *temp = (if_t*) ast;
	PRINT("IF");
	ast_print(temp->cond, fp);
	ast_print(temp->succ, fp);
	ast_print(temp->fail, fp);
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

static void delete_TYPE(ast_t *ast)
{
	type_t *t = (type_t *)ast;
	//TODO: Decide if have next
	ast_delete(t->name);
	free(ast);
}

static void delete_FOR(ast_t *ast)
{
	for_t *f = (for_t*)ast;
	ast_delete(f->init);
	ast_delete(f->cond);
	ast_delete(f->post);
	ast_delete(f->expr);
	free(f);
}

static void delete_ID(ast_t *ast)
{
	identifier_t *id = (identifier_t*)ast;
	free(id->name);
	free(id);
}

static void delete_ASSIGN(ast_t *ast)
{
	assign_t *a = (assign_t*)ast;
	delete_ID((ast_t*)a->id);
	ast_delete(a->expr);
	free(a);
}

static void delete_BINOP(ast_t *ast)
{
	binop_t *binop = (binop_t*)ast;
	ast_delete(binop->left);
	ast_delete(binop->right);
	free(binop);
}

static void delete_DECL(ast_t *ast)
{
	decl_t *d = (decl_t*)ast;
	ast_delete(d->t);
	ast_delete(d->name);
	ast_delete(d->expr);
	free(d);
}

static void delete_AST_LIST(ast_t *ast)
{
	ast_list_t *l = (ast_list_t*)ast;
	for(int i = 0; i < l->size; i++)
	{
		ast_delete(l->exprs[i]);
	}
	free(l->exprs);
	free(l);
}

static void delete_DECL_LIST(ast_t *ast)
{
	delete_AST_LIST(ast);
}

static void delete_DO(ast_t *ast)
{
	do_t *d = (do_t*)ast;
	ast_delete(d->cond);
	ast_delete(d->statement);
	free(d);
}

void delete_list_static(ast_list_t *l)
{
	for(int i = 0; i < l->size; i++)
	{
		ast_delete(l->exprs[i]);
	}
	free(l->exprs);
}

static void delete_FUNC_CALL(ast_t *ast)
{
	function_call_t *f = (function_call_t*)ast;
	ast_delete(f->call);

	ast_list_t *l = &f->params;
	delete_list_static(l);

	free(f);
}


static void delete_IF(ast_t *ast)
{
	if_t *f = (if_t*)ast;
	ast_delete(f->cond);
	ast_delete(f->succ);
	ast_delete(f->fail);
	free(f);
}
static void delete_LIST(ast_t *ast)
{
	delete_AST_LIST(ast);
}

static void delete_NONE(ast_t *ast)
{
	free(ast);
}

static void delete_IMPORT(ast_t *ast)
{
	import_t *i = (import_t*)ast;
	ast_delete(i->name);
	ast_delete(i->to_import);
	free(i);
}
static void delete_PROGRAM(ast_t *ast)
{
	program_t *program = (program_t *)ast;
	for(int i = 0; i < program->num_imports; i++)
	{
		ast_delete(program->imports[i]);
	}
	for(int i = 0; i < program->num_functions; i++)
	{
		ast_delete(program->functions[i]);
	}
	free(program->imports);
	free(program->functions);
	free(program);
}

static void delete_FUNCTION(ast_t *ast)
{
	function_t *func = (function_t *)ast;
	ast_delete(func->name);
	ast_delete(func->input);
	ast_delete(func->output);
	ast_delete(func->statements);
	free(func);
	//TODO:Delete the symbol table too.
}

static void delete_RETURN(ast_t *ast)
{
	return_t *r = (return_t*)ast;
	ast_delete(r->expr);
	free(r);
}


static void delete_STATEMENT_LIST(ast_t *ast)
{
	delete_AST_LIST(ast);
}

static void delete_INTEGER(ast_t *ast)
{
	literal_t *l = (literal_t*)ast;
	free(l);
}

static void delete_FLOAT(ast_t *ast)
{
	literal_t *l = (literal_t*)ast;
	free(l);
}

static void delete_STRING(ast_t *ast)
{
	literal_t *l = (literal_t*)ast;
	free(l->string);
	free(l);
}

static void delete_WHILE(ast_t *ast)
{
	while_t *w = (while_t*)ast;
	ast_delete(w->cond);
	ast_delete(w->statement);
	free(w);
}

#define X(NAME)\
	case AST_TYPE_ ## NAME:\
		print_ ## NAME(ast, fp);\
		break
void ast_print_(ast_t *ast, FILE *fp)
{
	if(ast == NULL) return;
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

#define X(NAME)\
	case AST_TYPE_ ## NAME:\
		delete_ ## NAME(ast);\
		break
void ast_delete_(ast_t *ast)
{
	if(ast == NULL) return;
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
}
#undef X
