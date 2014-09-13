#include "symbol.h"
#include <stdlib.h>

typedef struct scope
{
	int      size;
}scope_t;

struct symbol_table
{
	scope_t *scope;
};


// This function will scan for the given import
// and will then add any functions to the symbol table.
symbol_table_t *symbol_table_new(void)
{
	symbol_table_t *out = MALLOC_T(symbol_table_t);
	return out;
}
