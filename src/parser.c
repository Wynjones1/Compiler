#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "operators.h"
#include "list.h"
#include "parser.h"
#include "string_common.h"

#define DEBUG_PARSER (1)

parse_state_t *parse_state_init(token_list_t *tl, allocator_t *allocator)
{
    parse_state_t *out = malloc(sizeof(parse_state_t));
    out->al    = allocator;
    out->toks  = tl->tokens;
    out->count = tl->size;
    out->pos   = 0;
    return out;
}

static token_t *current_token(parse_state_t *ps)
{
    return ps->toks + ps->pos;
}

static token_t *next_token(parse_state_t *ps)
{
    token_t *out = current_token(ps);
    ps->pos += 1;
    return out;
}

static token_t *accept(parse_state_t *ps, enum TOKEN_TYPE type)
{
    if(current_token(ps)->type == type)
    {
        token_t *out = current_token(ps);
        ps->pos++;
        return out;
    }
    return NULL;
}

#define ACCEPT_OR_FAIL(ps, type)         \
    do                                   \
    {                                    \
        token_t *_x = accept(ps, type);  \
        if(_x == NULL)                   \
        {                                \
            return NULL;                 \
        }                                \
    }while(0);

#define ACCEPT_OR_FAIL_VAR(var, ps, type)\
    do                                   \
    {                                    \
        token_t *_x = accept(ps, type);  \
        if(_x == NULL)                   \
        {                                \
            return NULL;                 \
        }                                \
        var = _x;                        \
    }while(0);

ast_t *parse_(ast_t*(*func)(parse_state_t*), parse_state_t *ps)
{

    parse_state_t ps_new = *ps;
    ast_t *out = func(&ps_new);
    allocator_push(ps->al);
    if(out)
    {
        *ps = ps_new;
        allocator_forget(ps->al);
    }
    else
    {
        allocator_pop(ps->al);
    }
    return out;
}

#define PARSE(func, ps) parse_(parse_ ## func, ps);

#define IF_PARSE_RETURN(func, ps)      \
    do{                                \
        ast_t *_out = PARSE(func, ps); \
        if(_out)                       \
        {                              \
            return _out;               \
        }                              \
    } while(0);

ast_t *parse_type(parse_state_t *ps)
{
    ast_t *id = PARSE(id, ps);
    typedecl_t *td = allocator_new(ps->al, sizeof(typedecl_t));
    td->id = id;
    return ast_decl(td, ps);
}

ast_t *parse_param(parse_state_t *ps)
{
    ast_t *type = PARSE(type, ps);
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_COLON);
    ast_t *id = PARSE(id, ps);

    return ast_param(type, id, ps);
}

ast_t *parse_id(parse_state_t *ps)
{
    token_t *tok;
    ACCEPT_OR_FAIL_VAR(tok, ps, TOKEN_TYPE_ID);
    return ast_id(tok->value, ps);
}

ast_t *parse_integer_literal(parse_state_t *ps)
{
    token_t *tok;
    ACCEPT_OR_FAIL_VAR(tok, ps, TOKEN_TYPE_INT_LITERAL);
    return ast_int_literal(tok->value, ps);
}


ast_t *parse_expression_list(parse_state_t *ps)
{
    list_t *list = list_init(sizeof(ast_t*));
    do
    {
        ast_t *expr = PARSE(expression, ps);
        list_push(list, &expr);
    }
    while(accept(ps, TOKEN_TYPE_COMMA));
    /* Copy the data from the list to an AST list */
    ast_t *out = ast_list(list_count(list), list_data(list), ps);
    list_delete(list);
    return out;
}

ast_t *parse_func_call(parse_state_t *ps)
{
    ast_t *func = PARSE(id, ps);
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_LPAREN);
    ast_t *params = PARSE(expression_list, ps);
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_RPAREN);

    return ast_func_call(func, params, ps);
}

ast_t *parse_sub_expression(parse_state_t *ps)
{
    IF_PARSE_RETURN(func_call, ps)
    IF_PARSE_RETURN(id, ps);
    IF_PARSE_RETURN(integer_literal, ps);
    IF_PARSE_RETURN(paren_expr, ps);
}

ast_t *parse_operation(parse_state_t *ps)
{
    if(!is_operator(current_token(ps)->type))
    {
        return NULL;
    }
    token_t *op = next_token(ps);
    // The actual tree nodes will be filled in later.
    return ast_binary_op((enum OPERATOR_TYPE)op->type, ps);
}

struct sy_queue_stack
{
    size_t size;
    ast_t *elems[1024];
};

void queue_push(struct sy_queue_stack *queue, ast_t *ast)
{
    assert(queue->size< sizeof(queue->elems) / sizeof(*queue->elems));
    queue->elems[queue->size] = ast;
    queue->size++;
}

ast_t *queue_pop(struct sy_queue_stack *queue)
{
    assert(queue->size);
    queue->size--;
    ast_t *out = queue->elems[0];
    memmove(queue->elems, queue->elems + 1, sizeof(ast_t*) * queue->size);
    return out;
}

void stack_push(struct sy_queue_stack *stack, ast_t *ast)
{
    assert(stack->size < sizeof(stack->elems) / sizeof(*stack->elems));
    stack->elems[stack->size] = ast;
    stack->size++;
}

ast_t *stack_pop(struct sy_queue_stack *stack)
{
    assert(stack->size);
    stack->size--;
    return stack->elems[stack->size];
}

ast_t *stack_top(struct sy_queue_stack *stack)
{
    if(stack->size)
    {
        return stack->elems[stack->size - 1];
    }
    return NULL;
}

static enum OPERATOR get_op_type(ast_t *ast)
{
    return ((ast->type == AST_TYPE_BINARY_OPERATION) ? ast->binary_op.type : ast->unary_op.type);
}
int get_precedence(ast_t *op)
{
    enum OPERATOR type = get_op_type(op);
#define X(NAME, PREC, ASSOC) if(type == OPERATOR_ ## NAME) return PREC;
    X_OPERATOR_LIST
#undef X
    exit(-1);
}

enum ACCOCIATIVITY get_assoc(ast_t *op)
{
    enum OPERATOR type = get_op_type(op);
#define X(NAME, PREC, ASSOC) if(type == OPERATOR_ ## NAME) return ASSOC;
    X_OPERATOR_LIST
#undef X
    exit(-1);
}

ast_t *parse_expression(parse_state_t *ps)
{
    struct sy_queue_stack queue = {.size = 0};
    struct sy_queue_stack stack = {.size = 0};

    while(true)
    {
        // Process Expression.
        ast_t *expr = PARSE(sub_expression, ps);
        if(expr)
        {
            queue_push(&queue, expr);
            continue;
        }
        // Process Operations.
        ast_t *o1 = PARSE(operation, ps);
        if(o1)
        {
            // while operators on the operator stack.
            ast_t *o2;
            while( (o2 = stack_top(&stack)) )
            {
                if(   (   get_assoc(o1) == LEFT  &&    get_precedence(o1) >= get_precedence(o2))
                   || (/* get_assoc(o2) == RIGHT && */ get_precedence(o1) >  get_precedence(o2)))
                {
                    queue_push(&queue, o2);
                    stack_pop(&stack);
                }
            }

            // push o1 to the stack.
            stack_push(&stack, o1);
            continue;
        }
        break;
    }
    // Put the remaining operations on the queue.
    while(stack.size)
    {
        queue_push(&queue, stack_pop(&stack));
    }
    ast_t *t0 = NULL, *t1, *t2;

    // There may be nothing here if the parse failed.
    if(queue.size != 0)
    {
        t0 = queue_pop(&queue);
        while(queue.size)
        {
            t1 = queue_pop(&queue);
            t2 = queue_pop(&queue);
            t2->binary_op.arg_0 = t0;
            t2->binary_op.arg_1 = t1;
            t0 = t2;
        }
    }
    return t0;
}

ast_t *parse_return(parse_state_t *ps)
{
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_KW_RETURN);
    ast_t *expr = PARSE(expression, ps);
    ast_t *out  = ast_return(expr, ps);
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_SEMICOLON);
    return out;
}

ast_t *parse_variable_declaration(parse_state_t *ps)
{
    ast_t *type = PARSE(type, ps);
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_COLON);
    ast_t *name = PARSE(id, ps);
    ast_t *expr = NULL;
    if(accept(ps, TOKEN_TYPE_ASSIGN))
    {
        expr = PARSE(expression, ps);
    }
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_SEMICOLON);

    // Create the variable declaration node.
    return ast_vardecl(type, name, expr, ps);
}

ast_t *parse_paren_expr(parse_state_t *ps)
{
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_LPAREN)
    ast_t *expr = PARSE(expression, ps);
    if(expr != NULL)
    {
        ACCEPT_OR_FAIL(ps, TOKEN_TYPE_RPAREN);
    }
    return expr;
}

ast_t *parse_braced_stmt_list(parse_state_t *ps)
{
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_LBRACE);
    ast_t *out = PARSE(statement_list, ps);
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_RBRACE);
    return out;
}


ast_t *parse_if_common(parse_state_t *ps)
{
    ast_t *cond    = PARSE(paren_expr, ps);
    ast_t *success = PARSE(braced_stmt_list, ps);
    ast_t *fail    = NULL;
    if(accept(ps, TOKEN_TYPE_KW_ELSE))
    {
        fail = PARSE(braced_stmt_list, ps);
    }
    else if(accept(ps, TOKEN_TYPE_KW_ELIF))
    {
        fail = PARSE(if_common, ps);
    }

    // Create the "if"/"else"/"elif" node.
    return ast_if(cond, success, fail, ps);
}


ast_t *parse_if(parse_state_t *ps)
{
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_KW_IF)
    return parse_if_common(ps);
}


ast_t *parse_while(parse_state_t *ps)
{
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_KW_WHILE)
    ast_t *cond = PARSE(paren_expr, ps);
    ast_t *stmt = PARSE(braced_stmt_list, ps);

    // Create the "while" node
    return ast_while(cond, stmt, ps);
}

ast_t *parse_expression_statement(parse_state_t *ps)
{
    ast_t *expr = PARSE(expression, ps);
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_SEMICOLON);
    return expr;
}

ast_t *parse_statement(parse_state_t *ps)
{
    IF_PARSE_RETURN(return, ps);
    IF_PARSE_RETURN(variable_declaration, ps);
    IF_PARSE_RETURN(if, ps);
    IF_PARSE_RETURN(while, ps);
    IF_PARSE_RETURN(expression_statement, ps);

    return NULL;
}

ast_t *parse_list_generic(
            parse_state_t *ps,
            ast_t *(*func)(parse_state_t*),
            enum TOKEN_TYPE delimiter)
{
    list_t *list = list_init(sizeof(ast_t*));
    ast_t *out  = parse_(func, ps);
    while(out)
    {
        list_push(list, &out);
        if(   delimiter != TOKEN_TYPE_NONE 
           && !accept(ps, delimiter))
        {
            break;
        }
        out  = parse_(func, ps);
    }
    ast_t *out_list = ast_list(list_count(list), list_data(list), ps);
    list_delete(list);
    return out_list;
}


ast_t *parse_param_list(parse_state_t *ps)
{
    return parse_list_generic(ps, parse_param, TOKEN_TYPE_COMMA);
}


ast_t *parse_statement_list(parse_state_t *ps)
{
    return parse_list_generic(ps, parse_statement, TOKEN_TYPE_NONE);
}


ast_t *parse_function(parse_state_t *ps)
{
    // Check that is this actually a function.
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_KW_FUNCTION);

    // Get the functions name.
    ast_t *name = PARSE(id, ps);

    // Parse the parameters.
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_LPAREN);
    ast_t *params = PARSE(param_list, ps);
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_RPAREN);

    // Parse the return type.
    ast_t *return_ = NULL;
    if(accept(ps, TOKEN_TYPE_RARROW))
    {
        return_ = PARSE(type, ps);
    }

    // Parse the function body.
    ast_t *stmts = PARSE(braced_stmt_list, ps);

    // Create the function AST
    return ast_function(name, params, return_, stmts, ps);
}

ast_t *parse(token_list_t *tl, allocator_t *alloc)
{
    parse_state_t *ps = parse_state_init(tl, alloc);

    list_t *functions = list_init(sizeof(ast_t*));
        
    while(current_token(ps)->type != TOKEN_TYPE_NONE)
    {
        size_t start_pos = ps->pos;
        ast_t *function = parse_function(ps);
        list_push(functions, &function);
        if(ps->pos == start_pos)
        {
            fprintf(stderr, "Parse failed: No progress made.\n");
            exit(-1);
        }
    }
    ast_t *out = ast_list(list_count(functions), list_data(functions), ps);
    list_delete(functions);
    return out;
}
