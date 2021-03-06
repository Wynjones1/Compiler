#include "string_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *string_copy(const char *str, allocator_t *al)
{
    return string_copy_n(str, -1, al);
}

const char *string_copy_n(const char *str, size_t n, allocator_t *al)
{
    //TODO: check str is valid.
    size_t len = strlen(str);
    if(len > n)
    {
        len = n;
    }
    char *out;
    if(al == NULL)
    {
        out = malloc(len + 1);
        //TODO: error checking for malloc
    }
    else
    {
        out = allocator_new(al, len + 1);
    }
    memcpy(out, str, len);
    out[len] = '\0';
    return out;
}

void string_delete(const char *str)
{
    free((char*)str);
}

bool string_equal(const char *s0, const char *s1)
{
    return (strcmp(s0, s1) == 0);
}

const char *string_read_fp(FILE *fp)
{
    char buffer[1024];
    size_t pos = 0;
    char *out = malloc(1);
    do
    {
        size_t count = fread(buffer, 1, sizeof(buffer), fp);
        out = realloc(out, pos + count + 1);
        memcpy(out + pos, buffer, count);
        pos += count;
    }while(!feof(fp));

    out[pos] = '\0';
    return out;
}

const char *string_read_file(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    return string_read_fp(fp);
}

const char *string_append(const char *str0, const char *str1)
{
    size_t s0 = strlen(str0);
    size_t s1 = strlen(str1);
    char *out = realloc(str0, s0 + s1 + 1);
    memcpy(out + s0, str1, s1);
    out[s0 + s1] = '\0';
    return out;
}

const char *string_empty(void)
{
    return calloc(1, 1);
}