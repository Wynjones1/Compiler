#include "parser.h"


typedef struct parse_state parse_state_t;
struct parse_state
{
    token_t *toks;
    size_t   count;
    size_t   pos;
};

static parse_state_t parse_state_init(token_t *toks, size_t count)
{
    parse_state_t out;
    out.toks  = toks;
    out.count = count;
    out.pos   = 0;
    return out;
}

static token_t *current_token(parse_state_t *ps)
{
    return ps->toks + ps->pos;
}

static ast_t *parse_function(parse_state_t *ps)
{
    if(!accept(ps, TOKEN_TYPE_KW_FUNCTION))
        return NULL;

    accept(ps, TOKEN_TYPE_ID);

    // Parse the parameters.
    accept(ps, TOKEN_TYPE_LPAREN);
    ast_t *params = PARSE(param_list, ps);
    accept(ps, TOKEN_TYPE_RPAREN);

    // Parse the return type.
    accept(ps, TOKEN_TYPE_RARROW);
    ast_t *return_ = PARSE(type, ps);

    // Parse the function body.
    accept(ps, TOKEN_TYPE_LBRACE);
    ast_t *stmts = PARSE(statement_list, ps);
    accept(ps, TOKEN_TYPE_RBRACE);
}

ast_t *parse(token_t *list, size_t num_toks)
{
    parse_state_t ps = parse_state_init(list, num_toks);

    ast_t *functions = ast_list();
        
    while(current_token(&ps)->type != TOKEN_TYPE_NONE)
    {
        ast_t *function = parse_function(&ps);
        ast_list_append(functions, function);
    }

}
