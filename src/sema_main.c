#include "ast.h"
#include "sema.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_common.h"
#include "helpers.h"

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
    ast_t *ast = parse(tl);
    if(ast == NULL)
    {
        TODO_ERROR_HANDLING("Parse Failed.\n");
    }

    sema_state_t *ss = sema_state_init(al);
    sema(ast, ss);
    return 0;
}


