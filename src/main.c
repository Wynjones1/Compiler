#include <stdio.h>
#include "lexer.h"
#include "string_common.h"

int main(int argc, char **argv)
{
    const char *filename = argv[1];
    const char *file = string_read_file(filename);

    token_list_t *tl = tokenise(file);
    printf("Found %d tokens\n", (int)tl->size);
    for(size_t i = 0; i < tl->size; i++)
    {
        if(tl->tokens[i].value != NULL)
        {
            printf("%s\n", tl->tokens[i].value);
        }
    }
    return 0;
}
