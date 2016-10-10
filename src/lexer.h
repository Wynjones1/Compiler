#ifndef LEXER_H
#define LEXER_H
#include <stddef.h>
#include <stdint.h>

#define X_TOKEN_STRING_LIST       \
    X(TOKEN_TYPE_LPAREN,    "(" ) \
    X(TOKEN_TYPE_RPAREN,    ")" ) \
    X(TOKEN_TYPE_LBRACE,    "{" ) \
    X(TOKEN_TYPE_RBRACE,    "}" ) \
    X(TOKEN_TYPE_LSQUARE,   "[" ) \
    X(TOKEN_TYPE_RSQUARE,   "]" ) \
    X(TOKEN_TYPE_COLON,     ":" ) \
    X(TOKEN_TYPE_SEMICOLON, ";" ) \
    X(TOKEN_TYPE_COMMA    , "," ) \
    X(TOKEN_TYPE_ASSIGN,    "=" ) \
    X(TOKEN_TYPE_LARROW,    "<-") \
    X(TOKEN_TYPE_RARROW,    "->") \
    X(TOKEN_TYPE_OP_ADD,    "+" ) \
    X(TOKEN_TYPE_OP_SUB,    "-" ) \
    X(TOKEN_TYPE_OP_MUL,    "*" ) \
    X(TOKEN_TYPE_OP_DIV,    "/" ) \
    X(TOKEN_TYPE_OP_MOD,    "%" ) \
    X(TOKEN_TYPE_OP_LT,     "<" ) \
    X(TOKEN_TYPE_OP_LTE,    "<=") \
    X(TOKEN_TYPE_OP_GT,     ">" ) \
    X(TOKEN_TYPE_OP_GTE,    ">=") \
    X(TOKEN_TYPE_OP_EQ,     "==") \
    X(TOKEN_TYPE_OP_NEQ,    "!=") \

#define X_TOKEN_KEYWORD_LIST              \
    X(TOKEN_TYPE_KW_FUNCTION, "function") \
    X(TOKEN_TYPE_KW_RETURN,   "return")   \
    X(TOKEN_TYPE_KW_IF,       "if")       \
    X(TOKEN_TYPE_KW_ELIF,     "elif")     \
    X(TOKEN_TYPE_KW_ELSE,     "else")     \
    X(TOKEN_TYPE_KW_WHILE,    "while")

enum TOKEN_TYPE
{
    TOKEN_TYPE_NONE,
    TOKEN_TYPE_ID,
    TOKEN_TYPE_INT_LITERAL,
#define X(NAME, STRING) NAME,
    X_TOKEN_STRING_LIST
#undef X
#define X(NAME, STRING) NAME,
    X_TOKEN_KEYWORD_LIST
#undef X
    TOKEN_TYPE_WHITESPACE,
    TOKEN_TYPE_EOF,
    NUM_TOKENS,
};

typedef struct token
{
    enum TOKEN_TYPE  type;
    const char      *value;
    uint32_t         line;
    uint32_t         pos;
}token_t;

typedef struct token_list token_list_t;

/*  Initialise token
    Parameters:
        token - token to initialise, must point to valid memory. 
        type  - token type to initialise
        value - string value of the token. NULL indicates not value needed.
        size  - max size of the string to copy (not including null terminator).
*/
void token_init(token_t *token, enum TOKEN_TYPE type, const char *value, size_t size);

/*  Deinitalise token
    Parameters:
        token - token to deinitialise.
*/
void token_deinit(token_t *token);

/*  Tokenise input into token list.

    The last element in the list will always be a token of type
    TOKEN_TYPE_NONE.

    Parameters:
        data - string containing the source code.

    Returns:
        list of tokens.
*/
token_list_t *tokenise(const char *data);

token_list_t *tl_new();
void          tl_delete(token_list_t *);
token_t      *tl_get(token_list_t *tl, size_t index);
size_t        tl_size(token_list_t *tl);
void          tl_append(token_list_t *list, const char *data, enum TOKEN_TYPE type, size_t size);

#endif
