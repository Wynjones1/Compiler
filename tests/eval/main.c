#include "stdio.h"
#include "eval.h"
#include "string_common.h"

int main(int argc, char **argv)
{
    const char *input_string   = string_read_file(argv[1]);
    eval_result_t *er = eval_string(input_string);
    fprintf(stderr, "%s", er->stderr_);
    fprintf(stdout, "%s", er->stdout_);
    return er->retcode;
}
