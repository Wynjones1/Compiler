#include "eval.h"
#include "sema.h"
#include "parser.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "string_common.h"

#ifndef INPUT_FILE
#error "INPUT_FILE must be defined"
#endif


int main(int argc, char **argv)
{
    const char *input_string   = string_read_file(INPUT_FILE);

    token_list_t *tl = tokenise(input_string);
    allocator_t *alloc = allocator_init(1024);
    ast_t *ast = parse(tl, alloc);

    sema_state_t *ss = sema_state_init(alloc);
    sema(ast, ss);
    return 0;
}
