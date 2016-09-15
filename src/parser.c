#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "operators.h"
#include "parser.h"
#include "string_common.h"

#define DEBUG_PARSER (1)

parse_state_t parse_state_init(token_t *toks, size_t count)
{
    parse_state_t out;
    out.al    = allocator_init(1024);
    out.toks  = toks;
    out.count = count;
    out.pos   = 0;
    return out;
}

token_t *current_token(parse_state_t *ps)
{
    return ps->toks + ps->pos;
}

token_t *next_token(parse_state_t *ps)
{
    token_t *out = current_token(ps);
    ps->pos += 1;
    return out;
}

token_t *accept(parse_state_t *ps, enum TOKEN_TYPE type)
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
    token_t *token;
    ACCEPT_OR_FAIL_VAR(token, ps, TOKEN_TYPE_ID);
    ast_t *out = ast_make(AST_TYPE_TYPE_DECL);
    out->decl.id = string_copy(token->value);
    return out;
}

ast_t *parse_param(parse_state_t *ps)
{
    ast_t *type = PARSE(type, ps);
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_COLON);
    token_t *id;
    ACCEPT_OR_FAIL_VAR(id, ps, TOKEN_TYPE_ID);

    ast_t *out = ast_make(AST_TYPE_PARAM);
    out->param.type = type;
    out->param.name = string_copy(id->value);
    return out;
}

ast_t *parse_id(parse_state_t *ps)
{
    token_t *tok;
    ACCEPT_OR_FAIL_VAR(tok, ps, TOKEN_TYPE_ID);
    ast_t *out = ast_make(AST_TYPE_ID);
    out->int_literal = string_copy(tok->value);
    return out;
}

ast_t *parse_integer_literal(parse_state_t *ps)
{
    token_t *tok;
    ACCEPT_OR_FAIL_VAR(tok, ps, TOKEN_TYPE_INT_LITERAL);
    ast_t *out = ast_make(AST_TYPE_INT_LIT);
    out->int_literal = string_copy(tok->value);
    return out;
}


ast_t *parse_expression_list(parse_state_t *ps)
{
    ast_t *list = ast_list();
    do
    {
        ast_t *expr = PARSE(expression, ps);
        ast_list_append(list, expr);
    }
    while(accept(ps, TOKEN_TYPE_COMMA));
    return list;
}

ast_t *parse_func_call(parse_state_t *ps)
{
    ast_t *func = PARSE(id, ps);
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_LPAREN);
    ast_t *params = PARSE(expression_list, ps);
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_RPAREN);

    ast_t *out = ast_make(AST_TYPE_FUNC_CALL);
    out->func_call.func   = func;
    out->func_call.params = params;
    return out;
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
    ast_t *out = ast_make(AST_TYPE_OPERATION);
    out->op.type = op->type;
    return out;
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

int get_precedence(ast_t *op)
{
    enum OPERATOR type = op->op.type;
#define X(NAME, PREC, ASSOC) if(type == OPERATOR_ ## NAME) return PREC;
    X_OPERATOR_LIST
#undef X
    exit(-1);
}

enum ACCOCIATIVITY get_assoc(ast_t *op)
{
    enum OPERATOR type = op->op.type;
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
                if(     get_assoc(o1) == LEFT  &&    get_precedence(o1) >= get_precedence(o2)
                   || /*get_assoc(o2) == RIGHT && */ get_precedence(o1) >  get_precedence(o2))
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

    // There may be nothing here if we are not parsing an expression.
    if(queue.size != 0)
    {
        t0 = queue_pop(&queue);
        while(queue.size)
        {
            t1 = queue_pop(&queue);
            t2 = queue_pop(&queue);
            t2->op.arg_0 = t0;
            t2->op.arg_1 = t1;
            t0 = t2;
        }
    }
    return t0;
}

ast_t *parse_return(parse_state_t *ps)
{
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_KW_RETURN);
    ast_t *out = ast_make(AST_TYPE_RETURN);
    out->return_.expr = PARSE(expression, ps);
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_SEMICOLON);
    return out;
}

ast_t *parse_variable_declaration(parse_state_t *ps)
{
    ast_t *type = PARSE(type, ps);
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_COLON);
    token_t *name;
    ACCEPT_OR_FAIL_VAR(name, ps, TOKEN_TYPE_ID);
    ast_t *expr = NULL;
    if(accept(ps, TOKEN_TYPE_ASSIGN))
    {
        expr = PARSE(expression, ps);
    }
    ACCEPT_OR_FAIL(ps, TOKEN_TYPE_SEMICOLON);

    // Create the variable declaration node.
    ast_t *out = ast_make(AST_TYPE_VAR_DECL);
    out->vardecl.type = type;
    out->vardecl.name = string_copy(name->value);
    out->vardecl.expr = expr;
    return out;
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
        fail = parse_if_common(ps);
    }

    // Create the "if"/"else"/"elif" node.
    ast_t *out = ast_make(AST_TYPE_IF);
    out->if_.cond    = cond;
    out->if_.success = success;
    out->if_.fail    = fail;
    return out;
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
    ast_t *out = ast_make(AST_TYPE_WHILE);
    out->while_.cond = cond;
    out->while_.stmt = stmt;
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
    ast_t *list = ast_list();
    ast_t *out  = parse_(func, ps);
    while(out)
    {
        ast_list_append(list, out);
        if(   delimiter != TOKEN_TYPE_NONE 
           && !accept(ps, delimiter))
        {
            break;
        }
        out  = parse_(func, ps);
    }
    return list;
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
    token_t *id = accept(ps, TOKEN_TYPE_ID);

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
    ast_t *out = ast_make(AST_TYPE_FUNCTION);
    out->function.name        = string_copy(id->value);
    out->function.params      = params;
    out->function.return_     = return_;
    out->function.statements  = stmts;
    return out;
}

ast_t *parse(token_t *list, size_t num_toks)
{
    parse_state_t ps = parse_state_init(list, num_toks);

    ast_t *functions = ast_list();
        
    while(current_token(&ps)->type != TOKEN_TYPE_NONE)
    {
        parse_state_t ps_old = ps;
        ast_t *function = parse_function(&ps);
        ast_list_append(functions, function);
        if(ps.pos == ps_old.pos)
        {
            fprintf(stderr, "Parse failed: No progress made.\n");
            exit(-1);
        }
    }
    return functions;
}
