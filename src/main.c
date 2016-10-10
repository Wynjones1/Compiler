#include <stdio.h>
#include "lexer.h"
#include "string_common.h"

int main(int argc, char **argv)
{
    const char *filename = argv[1];
    const char *file = string_read_file(filename);

    token_list_t *tl = tokenise(file);
    printf("Found %d tokens\n", (int)tl_size(tl));
    for(size_t i = 0; i < tl_size(tl); i++)
    {
        if(tl_get(tl, i)->value != NULL)
        {
            printf("%s\n", tl_get(tl, i)->value);
        }
    }
    return 0;
}
