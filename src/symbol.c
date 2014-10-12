#include "symbol.h"
#include <stdlib.h>
#include <string.h>

typedef struct type_entry
{
	const char *key;
}type_entry_t;

typedef struct id_entry
{
	const char *key;
}id_entry_t;

struct symbol_table
{
	struct symbol_table *prev;

	type_entry_t  *types;
	unsigned int  num_types;

	id_entry_t    *ids;
	unsigned int  num_ids;
};


// This function will scan for the given import
// and will then add any functions to the symbol table.
symbol_table_t *symbol_table_new(void)
{
	symbol_table_t *out = MALLOC_T(symbol_table_t);
	out->prev      = NULL;
	out->types     = NULL;
	out->ids       = NULL;
	out->num_types = 0;
	out->num_ids   = 0;
	return out;
}

symbol_table_t *symbol_table_add(symbol_table_t *prev)
{
	symbol_table_t *out = MALLOC_T(symbol_table_t);
	out->prev = prev;
	return out;
}

static bool lookup_type_local(symbol_table_t *table, const char *type)
{
	for(unsigned int i = 0; i < table->num_types; i++)
	{
		if(strcmp(table->types[i].key, type) == 0) return true;
	}
	return false;
}

static bool lookup_id_local(symbol_table_t *table, const char *id)
{
	for(unsigned int i = 0; i < table->num_ids; i++)
	{
		if(strcmp(table->ids[i].key, id) == 0) return true;
	}
	return false;
}

bool symbol_table_lookup_type(symbol_table_t *table, const char *type)
{
	if(!lookup_type_local(table, type) && table->prev == NULL)
		return false;

	return symbol_table_lookup_type(table->prev, type);
}

bool symbol_table_add_type(symbol_table_t *table, const char *type)
{
	if(lookup_id_local(table, type) || lookup_type_local(table, type))
		return false;
	table->types = REALLOC_T(table->types, type_entry_t, ++table->num_types);
	table->types[table->num_types - 1].key = type;
	return true;
}

bool symbol_table_add_id(symbol_table_t *table, const char *id)
{
	if(lookup_id_local(table, id) || lookup_type_local(table, id))
		return false;
	table->ids = REALLOC_T(table->ids, id_entry_t, ++table->num_ids);
	table->ids[table->num_ids - 1].key = id;
	return true;
}

bool symbol_table_lookup_string(symbol_table_t *table, const char *string)
{
	return false;
}

void symbol_table_delete(symbol_table_t *table)
{}

bool symbol_table_add_var(symbol_table_t *table, decl_t *type)
{
	return false;
}

bool symbol_table_add_function(symbol_table_t *table, function_t *type)
{
	return false;
}
