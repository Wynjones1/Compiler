#include "eval.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    FILE *fp = fopen(INPUT_FILE, "r");
    if(fp == NULL)
    {
        fprintf(stderr, "Could not open file %s. (%s)\n", INPUT_FILE, strerror(errno));
        exit(-1);
    }

    char buffer[1024];
    char *data = NULL;
    size_t size = 0;
    while(!feof(fp))
    {
        size_t read = fread(buffer, 1, 1024, fp);
        data = realloc(data, size + read + 1);
        memcpy(data + size, buffer, read);
        size += read;
    }
    fclose(fp);

    char *input_data = data;
    char *output_data = NULL;
    for(size_t i = 2; i < size; i++)
    {
        if(    input_data[i - 2] == '\n'
            && input_data[i - 1] == '$'
            && input_data[i - 0] == '\n')
        {
            input_data[i - 1] = '\0';
            output_data  = input_data + i + 1;
        }
    }

    if(output_data == NULL)
    {
        fprintf(stderr, "Could not find test results\n");
        exit(-1);
    }

    data[size] = '\0';

    printf("============================================================================\n");
    printf("Testing file %s\n", INPUT_FILE);
    printf("================================(Input Data)================================\n");
    printf("%s\n", input_data);
    printf("==============================(Expected Output)=============================\n");
    printf("%s\n", output_data);

    const char *result = eval_string(input_data);
    if (strcmp(result, output_data) != 0)
    {
        printf("================================(Test Failed)===============================\n");
        printf("Result %s\n", result);
        printf("============================================================================\n");
        return 1;
    }

    printf("================================(Test Passed)===============================\n");
    printf("Result %s\n", result);
    printf("============================================================================\n");
    return 0;
}
