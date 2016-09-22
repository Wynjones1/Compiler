#include "eval.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "string_common.h"

#ifndef INPUT_FILE
#error "INPUT_FILE must be defined"
#endif

#ifndef STDERR_FILE
    #define STDERR_FILE (NULL)
#endif
#ifndef STDOUT_FILE
    #define STDOUT_FILE (NULL)
#endif
#ifndef RETCODE_FILE
    #define RETCODE_FILE (NULL)
#endif

int main(int argc, char **argv)
{
    const char *input_string   = string_read_file(INPUT_FILE);
    const char *stderr_string  = STDERR_FILE  ? string_read_file(STDERR_FILE) : NULL;
    const char *stdout_string  = STDOUT_FILE  ? string_read_file(STDOUT_FILE) : NULL;
    const char *retcode_string = RETCODE_FILE ? string_read_file(RETCODE_FILE): NULL;

    eval_result_t *result = eval_string(input_string);

    printf("============================================================================\n");
    printf("Testing file %s\n", INPUT_FILE);
    printf("================================(Input Data)================================\n");
    printf("%s\n", input_string);

    if(stdout_string != NULL)
    {
        printf("==============================(Expected stdout)=============================\n");
        printf("%s", stdout_string);
    }

    if(stderr_string != NULL)
    {
        printf("==============================(Expected stderr)=============================\n");
        printf("%s", stderr_string);
    }

    
    uint32_t expected_retcode = 0;
    if(retcode_string != NULL)
    {
        expected_retcode = (uint32_t)atoll(retcode_string);
    }

    printf("==============================(Expected retcode)=============================\n");
    printf("%d\n", expected_retcode);

    if(expected_retcode != result->retcode)
    {
        printf("================================(Test Failed)===============================\n");
        printf("retcode wrong, expected %d, got %d\n", expected_retcode, result->retcode);
        printf("============================================================================\n");
        return 1;
    }
    
    if (stdout_string && strcmp(stdout_string, result->stdout_) != 0)
    {
        printf("================================(Test Failed)===============================\n");
        printf("stdout mismatch...\n");
        printf("Expected:\n%s", stdout_string);
        printf("============================================================================\n");
        printf("Got:\n%s", result->stdout_);
        printf("============================================================================\n");
        return 1;
    }

    if (stderr_string && strcmp(stderr_string, result->stderr_) != 0)
    {
        printf("================================(Test Failed)===============================\n");
        printf("stderr mismatch...\n");
        printf("Expected:\n%s", stderr_string);
        printf("============================================================================\n");
        printf("Got:\n%s", result->stderr_);
        printf("============================================================================\n");
        return 1;
    }

    printf("===============================(Test Passed!!)==============================\n");
    return 0;
}
