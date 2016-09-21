#include "eval.h"
#include <stdio.h>
#include <stdlib.h>
#include "string_common.h"
#include "parser.h"
#include <assert.h>
#include <stdarg.h>
#include <string.h>

typedef struct symbol_table symbol_table_t;

#define TABLE_SIZE (100)

struct symbol_table
{
    struct
    {
        const char *key;
        ast_t      *value;
    }data[TABLE_SIZE];
    size_t size;
    symbol_table_t *parent;
};

void eval_error(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    exit(-1);
}

symbol_table_t *symtab_init(symbol_table_t *parent)
{
    symbol_table_t *out = malloc(sizeof(symbol_table_t));
    out->size= 0;
    out->parent = parent;
    return out;
}

void symtab_delete(symbol_table_t *table)
{}

struct eval_state
{
    symbol_table_t *table;
    allocator_t    *al;
    bool            returned;
    ast_t          *return_;
};

uint32_t get_value(ast_t *ast)
{
    assert(ast->type == AST_TYPE_INT_LIT);
    return (uint32_t)atoll(ast->string);
}

ast_t *query_table(symbol_table_t *table, const char *symbol)
{
    while(table != NULL)
    {
        for(size_t i = 0; i < table->size; i++)
        {
            if(string_equal(table->data[i].key, symbol))
            {
                return table->data[i].value;
            }
        }
        table = table->parent;
    }
    eval_error("Could not find symbol \"%s\" in table.\n", symbol);
}

eval_state_t *eval_state_init(allocator_t *al)
{
    eval_state_t *out = malloc(sizeof(eval_state_t));
    out->table    = symtab_init(NULL);
    out->al       = al;
    out->return_  = NULL;
    out->returned = false;
    return out;
}

void eval_state_add_scope(eval_state_t *state)
{
    state->table = symtab_init(state->table);
}

void table_add_entry(symbol_table_t *table, const char *key, ast_t *value)
{
    assert(table->size < TABLE_SIZE);
    table->data[table->size].key   = key;
    table->data[table->size].value = value;
    table->size++;
}

void table_update_entry(symbol_table_t *table, const char *key, ast_t *value)
{
    while(table != NULL)
    {
        for(size_t i = 0; i < table->size; i++)
        {
            if(string_equal(table->data[i].key, key))
            {
                table->data[i].value = value;
                return;
            }
        }
        table = table->parent;
    }
    eval_error("Could not find symbol \"%s\" in table.\n", key);
}

ast_t *eval_FUNCTION(ast_t *ast, eval_state_t *state)
{
    table_add_entry(state->table, ast->function.name->string, ast);
}

ast_t *eval_TYPE_DECL(ast_t *ast, eval_state_t *state)
{
    if(strcmp(ast->decl->id->string, "int") != 0)
    {
        eval_error("Only int is supported currently\n");
    }
}

ast_t *eval_PARAM(ast_t *ast, eval_state_t *state)
{}

ast_t *eval_LIST(ast_t *ast, eval_state_t *state)
{
    for(size_t i = 0; i < ast->list.count; i++)
    {
        eval(ast->list.data[i], state);
    }
}

ast_t *eval_RETURN(ast_t *ast, eval_state_t *state)
{
    state->return_  = eval(ast->return_.expr, state);
    state->returned = true;
}

ast_t *eval_ID(ast_t *ast, eval_state_t *state)
{
    return query_table(state->table, ast->string);
}

ast_t *eval_INT_LIT(ast_t *ast, eval_state_t *state)
{
    return ast;
}

ast_t *eval_VAR_DECL(ast_t *ast, eval_state_t *state)
{
    ast_t *value = ast->vardecl.expr ? eval(ast->vardecl.expr, state) : NULL;
    table_add_entry(state->table, ast->vardecl.name->string, value);
}

ast_t *eval_WHILE(ast_t *ast, eval_state_t *state)
{
    ast_t *cond = eval(ast->if_.cond, state);
    while(!state->returned && get_value(cond))
    {
        eval(ast->while_.stmt, state);
        cond = eval(ast->if_.cond, state);
    }
}

ast_t *eval_IF(ast_t *ast, eval_state_t *state)
{
    ast_t *cond = eval(ast->if_.cond, state);
    if(get_value(cond))
    {
        eval(ast->if_.success, state);
    }
    else
    {
        eval(ast->if_.fail, state);
    }
}

ast_t *eval_BINARY_OPERATION(ast_t *ast, eval_state_t *state)
{
    ast_t *lhs = eval(ast->binary_op.arg_0, state);
    ast_t *rhs = eval(ast->binary_op.arg_1, state);

    uint32_t lhs_value = get_value(lhs);
    uint32_t rhs_value = get_value(rhs);
    uint32_t out = 0;

    switch(ast->binary_op.type)
    {
    case OPERATOR_ADD:
        out = lhs_value + rhs_value;
        break;
    case OPERATOR_SUB:
        out = lhs_value - rhs_value;
        break;
    case OPERATOR_MUL:
        out = lhs_value * rhs_value;
        break;
    case OPERATOR_DIV:
        if(rhs_value == 0)
        {
            eval_error("Divison by zero error.\n");
        }
        out = lhs_value / rhs_value;
        break;
    case OPERATOR_MOD:
        if(rhs_value == 0)
        {
            eval_error("Divison by zero error.\n");
        }
        out = lhs_value % rhs_value;
        break;
    case OPERATOR_LT :
        out = lhs_value < rhs_value;
        break;
    case OPERATOR_LTE:
        out = lhs_value <= rhs_value;
        break;
    case OPERATOR_GT :
        out = lhs_value > rhs_value;
        break;
    case OPERATOR_GTE:
        out = lhs_value >= rhs_value;
        break;
    case OPERATOR_EQ :
        out = lhs_value == rhs_value;
        break;
    case OPERATOR_NEQ:
        out = lhs_value != rhs_value;
        break;
    default:
        eval_error("Unknown operation type %d\n", ast->binary_op.type);
    }

    char buffer[100];
    sprintf(buffer, "%lu", (unsigned long) out);
    ast_t *out_ast= malloc(sizeof(ast_t));
    out_ast->type = AST_TYPE_INT_LIT;
    out_ast->string = string_copy(buffer, NULL);
    return out_ast;
}

ast_t *eval_UNARY_OPERATION(ast_t *ast, eval_state_t *state)
{}

ast_t *eval_STATEMENT_LIST(ast_t *ast, eval_state_t *state)
{
    for(size_t i = 0; i < ast->function.statements->list.count; i++)
    {
        eval(ast->function.statements->list.data[i], state);
        if(state->returned)
        {
            break;
        }
    }
}

ast_t *eval_FUNC_CALL(ast_t *ast, eval_state_t *state)
{
    ast_t *id = ast->func_call.func;
    ast_t *func = query_table(state->table, id->string);
    ast_t *func_params = func->function.params;
    ast_t *call_params = ast->func_call.params;
    if(func_params->list.count != call_params->list.count)
    {
        fprintf(stderr, "Mismatch in number of parameters.\n");
        exit(-1);
    }

    symbol_table_t *table = state->table;
    state->table = symtab_init(NULL);
    for(size_t i = 0; i < func_params->list.count; i++)
    {
       ast_t *param = eval(call_params->list.data[i], state);
       table_add_entry(state->table, func_params->list.data[i]->param.name->string, param);
    }

    eval(ast->function.statements, state);

    state->return_  = NULL;
    state->returned = false;

    eval(ast->function.statements, state);

    symtab_delete(state->table);
    state->table = table;

    return state->return_;
}

ast_t *eval(ast_t *ast, eval_state_t *state)
{
#define X(NAME)                           \
    if(ast->type == AST_TYPE_ ## NAME)    \
    {                                     \
        return eval_ ## NAME(ast, state); \
    }
    X_AST_TYPE_LIST
#undef X
    fprintf(stderr, "Encountered node with unknown type.\n");
    exit(-1);
}

ast_t *make_entry_node(allocator_t *alloc)
{
    parse_state_t *ps = parse_state_init(NULL, alloc);
    return ast_func_call(ast_id("main", ps), ast_list(0, NULL, ps), ps);
}

void eval_string(const char *string)
{
    token_list_t *tl = tokenise(string);
    allocator_t *alloc = allocator_init(1024);
    ast_t *ast  = parse(tl, alloc);
    ast_t *main_ast = make_entry_node(alloc);
    eval_state_t *state = eval_state_init(alloc);
    eval(ast, state);
    eval(main_ast, state);
}
