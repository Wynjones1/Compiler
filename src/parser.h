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

#endif
