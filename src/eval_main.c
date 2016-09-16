#include "ast.h"
#include "eval.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_common.h"

int main(int argc, char **argv)
{
    FILE *fp = stdin;
    if(argc == 2)
    {
        fp = fopen(argv[1], "r");
    }

    const char *data = string_read_fp(fp);

    token_list_t *tl = tokenise(data);
    string_delete(data);

    allocator_t *al = allocator_init(1024);
    ast_t *ast = parse(tl, al);
    if(ast == NULL)
    {
        fprintf(stderr, "Parse Failed.\n");
    }
    eval(ast, al);
    return 0;
}


