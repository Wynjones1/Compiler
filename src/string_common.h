#ifndef STRING_COMMON_H
#define STRING_COMMON_H
#include <stddef.h>
#include <stdbool.h>
#include "allocator.h"

/*  Copy a string
    Parameters:
        str - string to copy.

*/

const char *string_copy(const char *str, allocator_t *al);

/*  Copy a string up to a certain size

    This function will *always* NULL terminate.

    Parameters:
        str - string to copy.
        n   - copy at most n characters

*/
const char *string_copy_n(const char *str, size_t n, allocator_t *al);

/* Read a file into a string

    Parameters:
        filename - filename from which to read the string.
*/
const char *string_read_file(const char *filename);

void        string_delete(const char *str);

/* Returns true if the two strings are equal, false otherwise */
bool string_equal(const char *s0, const char *s1);

#endif
