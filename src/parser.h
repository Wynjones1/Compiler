#pragma once
#include "common.h"
#include "lexer.h"
#include "ast.h"

program_t *parse(token_t *tokens);
