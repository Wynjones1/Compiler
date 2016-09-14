#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include "ast.h"

/*  Parse a list of tokens into an AST
    Parameters:
        list - list of tokens.
        num_toks - number of tokens.

    Returns:
        AST of the program described by list.
*/
ast_t *parse(token_t *list, size_t num_toks);

typedef struct parse_state parse_state_t;
struct parse_state
{
    token_t *toks;
    size_t   count;
    size_t   pos;
};
parse_state_t parse_state_init(token_t *toks, size_t count);

ast_t *parse_braced_stmt_list(parse_state_t *ps);
ast_t *parse_expression(parse_state_t *ps);
ast_t *parse_expression_list(parse_state_t *ps);
ast_t *parse_expression_statement(parse_state_t *ps);
ast_t *parse_func_call(parse_state_t *ps);
ast_t *parse_function(parse_state_t *ps);
ast_t *parse_id(parse_state_t *ps);
ast_t *parse_if(parse_state_t *ps);
ast_t *parse_if_common(parse_state_t *ps);
ast_t *parse_integer_literal(parse_state_t *ps);
ast_t *parse_operation(parse_state_t *ps);
ast_t *parse_param(parse_state_t *ps);
ast_t *parse_param_list(parse_state_t *ps);
ast_t *parse_paren_expr(parse_state_t *ps);
ast_t *parse_return(parse_state_t *ps);
ast_t *parse_statement(parse_state_t *ps);
ast_t *parse_statement_list(parse_state_t *ps);
ast_t *parse_sub_expression(parse_state_t *ps);
ast_t *parse_sub_expression(parse_state_t *ps);
ast_t *parse_variable_declaration(parse_state_t *ps);
ast_t *parse_while(parse_state_t *ps);

#endif
