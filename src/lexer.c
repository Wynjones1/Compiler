#include "lexer.h"
#include <stdlib.h>
#include "string_common.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

void token_init(token_t *token, enum TOKEN_TYPE type, const char *value, size_t size)
{
    token->type = type;
    if(value != NULL)
    {
        token->value = string_copy_n(value, size);
    }
    else
    {
        token->value = NULL;
    }
}

void token_deinit(token_t *token)
{
    string_delete(token->value);
}

static token_t *list_init(void)
{
    return NULL;
}

/*  Append token data to a token list.

    Parameters:
        tok   - pointer to the token list.
        data  - start of the token string.
        type  - token type.
        size  - size of the token string.
        count - size of the list.
*/
static void list_append(token_t **tok, const char *data, enum TOKEN_TYPE type, size_t size, size_t count)
{
    *tok = realloc(*tok, sizeof(token_t) * (count + 1));
    token_init((*tok) + count, type, data, size);
}

static enum TOKEN_TYPE get_id_type(const char *data, size_t size)
{
    // Check each keyword, if the ID matches return its type.
#define X(TYPE, STRING)                                    \
    if(    sizeof(STRING) == (size + 1)                    \
        && strncmp(data, STRING, sizeof(STRING) - 1) == 0) \
    {                                                      \
        return TYPE;                                       \
    }
    X_TOKEN_KEYWORD_LIST
#undef X
    // Must not have been a keyword. So it's an ID.
    return TOKEN_TYPE_ID;
}

static bool accept_identifier(const char *data, size_t *offset, enum TOKEN_TYPE *type)
{
    size_t size = 0;
    // [A-Za-z_][A-Za-z0-9_]*
    if(isalpha(data[0]))
    {
        size++;
        while(isalnum(data[size]) || data[size] == '_')
        {
            size++;
        }
        *type   = get_id_type(data, size);
        *offset = size;
        return true;
    }
    return false;
}

static bool accept_operator(const char *data, size_t *offset, enum TOKEN_TYPE *type_)
{
    #define X(type, strings) strings,
    const char *strings[] = {
        X_TOKEN_STRING_LIST
    };
    #undef X

    #define X(type, strings) type,
    enum TOKEN_TYPE types[] = {
        X_TOKEN_STRING_LIST
    };
    #undef X

    size_t size = 0;
    enum TOKEN_TYPE type;
    for(size_t i = 0; i < sizeof(types) / sizeof(*types); i++)
    {
        size_t len = strlen(strings[i]);
        if(len > size && strncmp(data, strings[i], len) == 0)
        {
            size = len;
            type = types[i];
        }
    }
    if(size)
    {
        *offset = size;
        *type_ = type;
        return true;
    }
    return false;
}

static bool accept_integer_literal(const char *data, size_t *offset, enum TOKEN_TYPE *type)
{
    size_t count = 0;
    while(isdigit(data[count]))
    {
        count++;
    }
    if(count)
    {
        *offset = count;
        *type   = TOKEN_TYPE_INT_LITERAL;
        return true;
    }
    return false;
}

static bool accept_whitespace(const char *data, size_t *offset, enum TOKEN_TYPE *type)
{
    size_t count = 0;
    while(isspace(data[count]))
    {
        count++;
    }
    if(count)
    {
        *offset = count;
        *type   = TOKEN_TYPE_WHITESPACE;
        return true;
    }
    return false;
}

void tokenise(const char *data_, token_t **list_, size_t *count_)
{
    const char *data = data_;
    size_t size  = 0;
    size_t count = 0;
    enum TOKEN_TYPE type;
    token_t *list = list_init();
    while(data[0] != '\0')
    {
        if(    accept_identifier(data, &size, &type)
            || accept_operator(data, &size, &type)
            || accept_integer_literal(data, &size, &type)
            || accept_whitespace(data, &size, &type))
        {
            if(type != TOKEN_TYPE_WHITESPACE)
            {
                list_append(&list, data, type, size, count);
                count += 1;
            }
            data  += size;
        }
        else
        {
            fprintf(stderr, "Unrecognised character (%c)\n", data[0]);
            exit(-1);
        }
    }
    list_append(&list, NULL, TOKEN_TYPE_NONE, 0, count);
    *list_  = list;
    *count_ = count + 1;
}
