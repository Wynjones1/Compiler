#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    FILE *fp = stdin;
    if(argc == 2)
    {
        fp = fopen(argv[1], "r");
    }

    char buffer[1024];
    char *data = NULL;
    size_t size = 0;
    while(!feof(fp))
    {
        size_t read = fread(buffer, 1, 1024, fp);
        data = realloc(data, size + read + 1);
        memcpy(data + size, buffer, read);
        size += read;
    }

    data[size] = '\0';

    token_list_t *tl = tokenise(data);
    printf("Found %lu tokens\n", tl->size);
    return 0;
}
