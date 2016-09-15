#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <stdlib.h>
#include "ast.h"
#include "parser.h"
#include <errno.h>

typedef ast_t *(*parse_function_t)(parse_state_t *);

parse_function_t load_parse_function(const char *function_name)
{
    void *handle = dlopen("libcompilerlib.so", RTLD_LAZY);
    if(!handle)
    {
        fprintf(stderr, "Could not open libcompilerlib.so");
        exit(-1);
    }

    parse_function_t func = dlsym(handle, function_name);
    char *error;
    if((error = dlerror()) != NULL)
    {
        fprintf(stderr, "Error opening function %s (%s)\n", function_name, error);
        exit(-1);
    }
    return func;
}

int main(int argc, char **argv)
{

    if(argc < 3)
    {
        fprintf(stderr, "Must supply (function_name, filename)\n");
        exit(-1);
    }

    const char *function_name = argv[1];
    //const char *function_name = "parse_id";

    parse_function_t func = load_parse_function(function_name);

    FILE *fp = fopen(argv[2], "r");
    if(fp == NULL)
    {
        fprintf(stderr, "Could not open file %s (%s).\n", argv[2], strerror(errno));
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

    data[size] = '\0';

    printf("============================================================================\n");
    printf("Testing function %s\n", function_name);
    printf("================================(Input Data)================================\n");
    printf("%s\n", data);
    printf("============================================================================\n");

    token_list_t *token_list = tokenise(data);
    parse_state_t ps = parse_state_init(token_list->tokens, token_list->size);
    ast_t *out = func(&ps);
    if(out == NULL)
    {
        printf("Failed\n");
        return 1;
    }
    printf("Success\n");
    token_list_delete(token_list);
    return 0;
}
