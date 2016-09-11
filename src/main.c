#include <stdio.h>
#include "lexer.h"
#include "string_common.h"

int main(int argc, char **argv)
{
    const char *filename = argv[1];
    const char *file = string_read_file(filename);

    token_t *tokens;
    size_t count;
    tokenise(file, &tokens, &count);
    printf("Found %lu tokens\n", count);
    for(size_t i = 0; i < count; i++)
    {
        if(tokens[i].value != NULL)
        {
            printf("%s\n", tokens[i].value);
        }
    }
    return 0;
}
