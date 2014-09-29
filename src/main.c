#include <stdio.h>
#include <string.h>
#include "compiler.h"

int cmp(const char *a, const char *b)
{
	return strncmp(a, b, strlen(a)) == 0;
}
#define HAVE_NEXT (i + 1 < argc)

int main(int argc, char *argv[])
{
	const char *input = "./tests/binop1.c";
	const char *output = "a.out";
	for(int i = 1; i < argc; i++)
	{
		const char *arg = argv[i];
		if(cmp("-i", arg) && HAVE_NEXT)
		{
			input = argv[i++ + 1];
		}
		if(cmp("-o", arg) && HAVE_NEXT)
		{
			output = argv[i++ + 1];
		}
	}
	if(input)
	{
		compile(input, output);
	}
	else
	{
		printf("Not input file.\n");
	}
	return 0;
}
