#include "ast.h"
#include "eval.h"
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

    ast_t *ast = parse(tl);
    if(ast == NULL)
    {
        TODO_ERROR_HANDLING("Parse Failed.\n");
    }

    eval_state_t *es = eval_state_init();
    eval(ast, es);

    ast_t *entry_point = make_entry_node();
    eval(entry_point, es);
    return 0;
}


