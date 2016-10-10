#include "eval.h"
#include "sema.h"
#include "intermediate.h"
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
    ast_t *ast = parse(tl);

    sema_state_t *ss = sema_state_init(alloc);
    sema(ast, ss);

    ir_gen_state_t *is = intermediate_state_init(alloc);
    ir_t *ir = ir_gen(ast, is);
    return 0;
}
