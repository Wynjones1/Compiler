#ifndef STRING_COMMON_H
#define STRING_COMMON_H
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
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

    Return:
        string with contents of the file.
*/
const char *string_read_file(const char *filename);

/* Read a file into a string

    Parameters:
        fp - file pointer to file open with "r" mode.

    Return:
        string with contents of the file.
*/
const char *string_read_fp(FILE *fp);

void        string_delete(const char *str);

/* Returns true if the two strings are equal, false otherwise */
bool string_equal(const char *s0, const char *s1);

/* Concatonate two strings.
    Parameters:
        str0 - zero terminated string to be concatonated to
        str1 - zero terminated string to concatonate to str0

    After this call str0 is not guaranteed to point to valid memory.

    Return:
        new zero terminated string (allocated) that is the
*/
const char *string_append(const char *str0, const char *str1);

/* Create an empty string.

    Return:
        zero terminated empty string.
*/
const char *string_empty(void);

#endif
