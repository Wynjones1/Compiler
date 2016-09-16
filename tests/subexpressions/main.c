#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ast.h"
#include "parser.h"
#include "string_common.h"
#include <errno.h>

int main(void)
{
    FILE *fp = fopen(PARSE_FILE, "r");
    if(fp == NULL)
    {
        fprintf(stderr, "Could not open file %s (%s).\n", PARSE_FILE, strerror(errno));
        exit(-1);
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
    fclose(fp);

    data[size] = '\0';

    printf("============================================================================\n");
    printf("Testing function %s\n", PARSE_FILE);
    printf("================================(Input Data)================================\n");
    printf("%s\n", data);
    printf("============================================================================\n");

    token_list_t *token_list = tokenise(data);

    free(data);

    allocator_t *allocator = allocator_init(1024);
    parse_state_t *ps = parse_state_init(token_list, allocator);
    ast_t *out = PARSE_FUNC(ps);
    if(out == NULL)
    {
        printf("Failed\n");
        return 1;
    }
    printf("Success\n");
    token_list_delete(token_list);
    allocator_delete(allocator);
    return 0;
}
