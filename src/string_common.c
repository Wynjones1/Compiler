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
    }
    else
    {
        out = allocator_new(al, n + 1);
    }
    //TODO: error checking for malloc
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
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *out = malloc(size + 1);
    fread(out, size, 1, fp);
    out[size] = '\0';
    return out;
}

const char *string_read_file(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    return string_read_fp(fp);
}
