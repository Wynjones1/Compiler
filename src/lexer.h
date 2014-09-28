#pragma once
#include "common.h"
#include <stdio.h>
#define VERBOSE_LEXER 0

enum TOK
{
	TOK_ASSIGN,
	TOK_LPAREN,
	TOK_RPAREN,
	TOK_LBRACE,
	TOK_RBRACE,
	TOK_INTEGER,
	TOK_FLOAT,
	TOK_STRING,
	TOK_OP,
	TOK_ARROW,
	TOK_ID,
	TOK_SEMICOLON,
	TOK_COLON,
	TOK_COMMA,
	TOK_PERIOD,
	TOK_NEWLINE,
	TOK_KEYWORD,
	TOK_NONE,
	NUM_TOKENS
};

//WARNING: if you change this, you must chage
//the keyword string array.
enum KW
{
	KW_BREAK,
	KW_CASE,
	KW_CONTINUE,
	KW_DEFAULT,
	KW_DO,
	KW_ELSE,
	KW_FOR,
	KW_FROM,
	KW_FUNCTION,
	KW_IF,
	KW_IMPORT,
	KW_NAMESPACE,
	KW_RETURN,
	KW_STRUCT,
	KW_TYPEDEF,
	KW_VOID,
	KW_WHILE,
	NUM_KEYWORDS
};

//IMPORTANT!!! If changed you must alter string array in lexer.c too.
enum OP
{
	OP_NOT,
	OP_BIT_NOT,

	OP_MUL,
	OP_DIV,
	OP_MOD,

	OP_ADD,
	OP_SUB,

	OP_LSHIFT,
	OP_RSHIFT,

	OP_GT,
	OP_GTE,
	OP_LT,
	OP_LTE,

	OP_EQ,
	OP_NEQ,

	OP_BIT_AND,

	OP_BIT_XOR,

	OP_BIT_OR,

	OP_AND,

	OP_XOR,

	OP_OR,

	OP_QU,
};

const char *get_binop_string(enum OP op);

typedef struct token
{
	enum TOK type;
	union
	{
		float    flt;
		int      integer;
		char *   string;
		enum  OP op;
		enum  KW kw;
	};
	int line;
}token_t;

token_t *tokenise(FILE *fp);
void print_token(token_t *token);
int op_precedence(enum OP op);
