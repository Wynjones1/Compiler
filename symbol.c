#include "symbol.h"
#include <stdlib.h>

// This function will scan for the given import
// and will then add any functions to the symbol table.
symbol_table_t *symbol_table_new(void)
{
	symbol_table_t *out = malloc(sizeof(symbol_table_t));
	return out;
}
