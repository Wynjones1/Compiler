#pragma once
#include "common.h"

typedef struct symbol_table symbol_table_t;
typedef struct type type_t;
typedef struct function function_t;
typedef struct decl decl_t;

#define X(name) SYM_TABLE_ERRNO_ ## name
enum SYM_TABLE_ERRNO
{
	X(EXISTING_TYPE),
	X(EXISTING_ID)
};
#undef X


symbol_table_t *symbol_table_new();
void symbol_table_delete(symbol_table_t *table);
symbol_table_t *symbol_table_add(symbol_table_t *table);

bool            symbol_table_typedef(symbol_table_t *table, type_t *type);
bool            symbol_table_add_var(symbol_table_t *table, decl_t *type);
bool            symbol_table_add_function(symbol_table_t *table, function_t *type);

bool            symbol_table_lookup_string(symbol_table_t *table, const char *string);
bool            symbol_table_lookup_type(symbol_table_t *table, const char *name);
