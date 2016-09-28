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

ast_t *symbol_table_get_local(symbol_table_t *st, ast_t *key)
{
    for (size_t i = 0; i < st->size; i++)
    {
        if (compare_keys(key, st->data[i].key) == 0)
        {
            return st->data[i].value;
        }
    }
    return NULL;
}

ast_t *symbol_table_get(symbol_table_t *st, ast_t *id)
{
    symbol_table_t *table = st;
    while (table)
    {
        ast_t *value = symbol_table_get_local(table, id);
        if (value)
        {
            return value;
        }
        table = table->parent;
    }
    TODO_ERROR_HANDLING("Could not find symbol %s.\n", id->string);
}

void symbol_table_insert(symbol_table_t *st, ast_t *key, ast_t *value)
{
    if (symbol_table_get_local(st, key))
    {
        TODO_ERROR_HANDLING("Symbol %s already exists.\n", key->string);
    }

    if (st->size == st->capacity)
    {
        TODO_ERROR_HANDLING("Need to resize the symbol table.\n");
    }
    st->data[st->size].key   = key;
    st->data[st->size].value = value;
    st->size++;
}