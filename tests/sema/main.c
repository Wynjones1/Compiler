#include "eval.h"
#include "sema.h"
#include "parser.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "string_common.h"

int main(int argc, char **argv)
{
    if(argc == 1) return 1;

    const char *input_string   = string_read_file(argv[1]);

    token_list_t *tl = tokenise(input_string);
    allocator_t *alloc = allocator_init(1024);
    ast_t *ast = parse(tl);

    sema_state_t *ss = sema_state_init(alloc);
    sema(ast, ss);
    return 0;
}
