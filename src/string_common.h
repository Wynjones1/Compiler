#ifndef STRING_COMMON_H
#define STRING_COMMON_H
#include <stddef.h>

/*  Copy a string
    Parameters:
        str - string to copy.

*/

const char *string_copy(const char *str);

/*  Copy a string up to a certain size

    This function will *always* NULL terminate.

    Parameters:
        str - string to copy.
        n   - copy at most n characters

*/
const char *string_copy_n(const char *str, size_t n);

/* Read a file into a string

    Parameters:
        filename - filename from which to read the string.
*/
const char *string_read_file(const char *filename);

void        string_delete(const char *str);

#endif
