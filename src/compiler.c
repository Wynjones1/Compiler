#include "lexer.h"
#include "parser.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>


void compile(const char *input_file, const char *output_file)
{
	FILE *out_fp = fopen(output_file, "w");
	FILE *in_fp  = fopen(input_file, "r");
	if(!out_fp)
	{
		fprintf(stderr, "Could not open file %s: %s\n", output_file, strerror(errno));
		exit(-1);
	}
	if(!in_fp)
	{
		fprintf(stderr, "Could not open file %s: %s\n", input_file, strerror(errno));
		exit(-1);
	}
	token_t   *tokens  = tokenise(in_fp);
#if 1
	program_t *program = parse(tokens);
	FILE *fp = fopen("parse.tree", "w");
	printf("Finished Parsing.\n");
	fflush(stdout);
	ast_print(program, fp);
	ast_print(program, stdout);
#endif
	fclose(out_fp);
	fclose(in_fp);
}
