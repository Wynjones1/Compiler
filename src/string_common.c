#include "string_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *string_copy(const char *str)
{
    return string_copy_n(str, -1);
}

const char *string_copy_n(const char *str, size_t n)
{
    //TODO: check str is valid.
    size_t len = strlen(str);
    if(len > n)
    {
        len = n;
    }
    char *out = malloc(len + 1);
    //TODO: error checking for malloc
    memcpy(out, str, len);
    out[len] = '\0';
    return out;
}

void string_delete(const char *str)
{
    free((char*)str);
}

const char *string_read_file(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *out = malloc(size + 1);
    fread(out, size, 1, fp);
    out[size] = '\0';
    return out;
}
