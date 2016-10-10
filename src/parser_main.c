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
    if(data == NULL)
        TODO_ERROR_HANDLING("%s", data);
    token_list_t *tl = tokenise(data);

    allocator_t *alloc = allocator_init(1024);
    ast_t *d = parse(tl);
    return 0;
}


