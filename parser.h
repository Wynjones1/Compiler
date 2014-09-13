#pragma once
#include "lexer.h"
#include "ast.h"

ast_t *parse(token_t *tokens);
