#pragma once
#include <stdio.h>
#define VERBOSE_LEXER 0

enum TOK
{
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

enum KW
{
	KW_IF,
	KW_ELSE,
	KW_FUNCTION,
	KW_DO,
	KW_WHILE,
	KW_FROM,
	KW_IMPORT,
	KW_NAMESPACE,
	KW_RETURN,
	KW_BREAK,
	KW_CASE,
	KW_CONTINUE,
	KW_DEFAULT,
	KW_STRUCT,
	KW_TYPEDEF,
	KW_VOID,
	NUM_KEYWORDS
};

enum OP
{
	OP_ADD = '+',
	OP_SUB = '-',
	OP_MUL = '*',
	OP_DIV = '/',
	OP_LT = '<',
	OP_GT = '>',
	OP_EQ = '=',
	OP_QU = '?',
	OP_NOT = '~',
	OP_BANG = '!',
	OP_MOD = '%',
	OP_CARET = '^',
	OP_AND = '&',
	OP_OR = '|'
};

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
