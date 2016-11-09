#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "operators.h"
#include "list.h"
#include "parser.h"
#include "string_common.h"
#include "helpers.h"

#define DEBUG_PARSER (1)

#define PARSE(func, ps) parse_(parse_ ## func, ps);

#define ACCEPT_OR_FAIL(ps, type)         \
    do                                   \
    {                                    \
        token_t *_x = accept(ps, type);  \
        if(_x == NULL)                   \
        {                                \
            return NULL;                 \
        }                                \
    }while(0);

#define ACCEPT_OR_FAIL_VAR(var, ps, type) \
    do                                    \
    {                                     \
        token_t *_x = accept(ps, type);   \
        if(_x == NULL)                    \
        {                                 \
            return NULL;                  \
        }                                 \
        var = _x;                         \
    }while(0);

#define IF_PARSE_RETURN(func, ps)      \
    do{                                \
        ast_t *_out = PARSE(func, ps); \
        if(_out)                       \
        {                              \
            return _out;               \
        }                              \
    } while(0);

static token_t *current_token(parse_state_t *ps)
{
    return tl_get(ps->tl, ps->pos);
}

static token_t *next_token(parse_state_t *ps)
{
    token_t *out = tl_get(ps->tl, ps->pos);
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

ast_t *parse_array_qualifier(parse_state_t *ps)
{
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_LSQUARE);
    ast_t *type = PARSE(type, ps);
    if(type == NULL) return NULL;
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_COMMA);
    ast_t *size = PARSE(integer_literal, ps);
    if(size == NULL) return NULL;
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_RSQUARE);
    return ast_array_qualifier(type, size, ps->al);
}

ast_t *parse_type(parse_state_t *ps)
{
    IF_PARSE_RETURN(array_qualifier, ps);
    IF_PARSE_RETURN(id, ps);
    return NULL;
}

ast_t *parse_param(parse_state_t *ps)
{
    ast_t *type = PARSE(type, ps);
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_COLON);
    ast_t *id = PARSE(id, ps);

    return ast_param(type, id, ps->al);
}

ast_t *parse_id(parse_state_t *ps)
{
    token_t *tok;
    ACCEPT_OR_FAIL_VAR(tok, ps, TOKEN_TYPE_ID);
    return ast_id(tok->value, ps->al);
}

ast_t *parse_integer_literal(parse_state_t *ps)
{
    token_t *tok;
    ACCEPT_OR_FAIL_VAR(tok, ps, TOKEN_TYPE_INT_LITERAL);
    return ast_int_literal(tok->value, ps->al);
}

ast_t *parse_expression_list(parse_state_t *ps)
{
    list_t *list = list_init(sizeof(ast_t*));
    do
    {
        ast_t *expr = PARSE(expression, ps);
        if (expr == NULL)
        {
            break;
        }
        list_push(list, &expr);
    }
    while(accept(ps, TOKEN_TYPE_COMMA));
    /* Copy the data from the list to an AST list */
    ast_t *out = ast_list(list_count(list), list_data(list), ps->al);
    list_delete(list);
    return out;
}

ast_t *parse_func_call(parse_state_t *ps)
{
    ast_t *func = PARSE(id, ps);
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_LPAREN);
    ast_t *params = PARSE(expression_list, ps);
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_RPAREN);

    return ast_func_call(func, params, ps->al);
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
    return ast_binary_op((enum OPERATOR)op->type, ps->al);
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
#define X(NAME, PREC, NUM_ARGS, ASSOC) if(type == OPERATOR_ ## NAME) return PREC;
    X_OPERATOR_LIST
#undef X
    exit(-1);
}

enum ACCOCIATIVITY get_assoc(ast_t *op)
{
    enum OPERATOR type = get_op_type(op);
#define X(NAME, PREC, NUM_ARGS, ASSOC) if(type == OPERATOR_ ## NAME) return ASSOC;
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
            while(    (o2 = stack_top(&stack))
                  &&  (   (get_assoc(o1) == LEFT     &&    get_precedence(o1) >= get_precedence(o2))
                       || (/* get_assoc(o2) == RIGHT && */ get_precedence(o1) >  get_precedence(o2))))
            {
                queue_push(&queue, o2);
                stack_pop(&stack);
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

    // There may be nothing here if the parse failed.
    if(queue.size == 0)
    {
        return NULL;
    }

    ast_t *t0;
    while(queue.size)
    {
        t0 = queue_pop(&queue);
        if(t0->type == AST_TYPE_BINARY_OPERATION)
        {
            ast_t *rhs = stack_pop(&stack);
            ast_t *lhs = stack_pop(&stack);
            t0->binary_op.arg_0 = lhs;
            t0->binary_op.arg_1 = rhs;
        }
        else if(t0->type == AST_TYPE_UNARY_OPERATION)
        {
            ast_t *arg = stack_pop(&stack);
            t0->unary_op.arg = arg;
            stack_push(&stack, t0);
        }
        stack_push(&stack, t0);
    }
    assert(stack.size == 1);
    return stack.elems[0];
}

ast_t *parse_return(parse_state_t *ps)
{
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_KW_RETURN);
    ast_t *expr = PARSE(expression, ps);
    ast_t *out  = ast_return(expr, ps->al);
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
    return ast_vardecl(type, name, expr, ps->al);
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
    ast_t *child = PARSE(statement_list, ps);
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_RBRACE);
    return ast_scope(child, ps->al);
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
    return ast_if(cond, success, fail, ps->al);
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
    return ast_while(cond, stmt, ps->al);
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
    ast_t *out_list = ast_list(list_count(list), list_data(list), ps->al);
    list_delete(list);
    return out_list;
}

ast_t *parse_param_list(parse_state_t *ps)
{
    return parse_list_generic(ps, parse_param, TOKEN_TYPE_COMMA);
}

ast_t *parse_statement_list(parse_state_t *ps)
{
    ast_t *out = parse_list_generic(ps, parse_statement, TOKEN_TYPE_NONE);
    out->type = AST_TYPE_STATEMENT_LIST;
    return out;
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
    return ast_function(name, params, return_, stmts, ps->al);
}

ast_t *parse(token_list_t *tl)
{
    parse_state_t *ps = parse_state_new(tl);

    list_t *functions = list_init(sizeof(ast_t*));
        
    while(current_token(ps)->type != TOKEN_TYPE_EOF)
    {
        size_t start_pos = ps->pos;
        ast_t *function = parse_function(ps);
        list_push(functions, &function);
        if(ps->pos == start_pos)
        {
            TODO_ERROR_HANDLING("Parse failed: No progress made.\n");
        }
    }
    ast_t *out = ast_root(ps->al);
    out->root.child = ast_list(list_count(functions), list_data(functions), ps->al);
    list_delete(functions);
    return out;
}

parse_state_t *parse_state_new(token_list_t *tl)
{
    parse_state_t *out = malloc(sizeof(parse_state_t));
    out->tl  = tl;
    out->pos = 0;
    out->al  = allocator_init(1024);
    return out;
}

void parse_state_delete(parse_state_t *ps)
{
    free(ps);
}
