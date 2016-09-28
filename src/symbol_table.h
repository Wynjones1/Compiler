#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#include "ast.h"
typedef struct symbol_table symbol_table_t;

symbol_table_t *symbol_table_init(symbol_table_t *parent);
void symbol_table_delete(symbol_table_t *st);

ast_t *symbol_table_get(symbol_table_t *st, ast_t *id);
void   symbol_table_insert(symbol_table_t *st, ast_t *key, ast_t *value);

#endif