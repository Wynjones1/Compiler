#include "symbol_table.h"
#include "list.h"
#include <stdio.h>
#include "helpers.h"
#include <string.h>
#include <stdlib.h>

struct tag
{
    ast_t *key;
    ast_t *value;
};


struct symbol_table
{
    //TODO: Make dynamically allocated.
    struct tag *data;
    size_t size;
    size_t capacity;
    symbol_table_t *parent;
};

int compare_keys(ast_t *lhs, ast_t *rhs)
{
    assert(lhs->type == AST_TYPE_ID);
    assert(rhs->type == AST_TYPE_ID);
    return strcmp(lhs->string, rhs->string);
}

symbol_table_t *symbol_table_init(symbol_table_t *parent)
{
    symbol_table_t *out = malloc(sizeof(symbol_table_t));
    out->parent   = parent;
    out->size     = 0;
    out->capacity = 1024;
    out->data     = malloc(sizeof(struct tag) * out->capacity);
    return out;
}

void symbol_table_delete(symbol_table_t *st)
{
    free(st);
}

ast_t *symbol_table_get(symbol_table_t *st, ast_t *id)
{

}

void symbol_table_insert(symbol_table_t *st, ast_t *key, ast_t *value)
{
    for (size_t i = 0; i < st->size; i++)
    {
        if (compare_keys(key, st->data[i].key) == 0)
        {
            TODO_ERROR_HANDLING("Key %s already exists in the symbol table\n", key->string);
        }
    }
    if (st->size == st->capacity)
    {

    }
}