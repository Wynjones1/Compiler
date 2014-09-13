#pragma once

typedef struct scope
{
	int      size;
}scope_t;

typedef struct symbol_table
{
	scope_t *scope;
}symbol_table_t;

symbol_table_t *symbol_table_new();
