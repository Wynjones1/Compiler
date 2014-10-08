#include <stdio.h>
#include <string.h>
#include "compiler.h"

#if RUN_TESTS
	#include "../tests/success.list"
	#include "../tests/fail.list"
#endif

int cmp(const char *a, const char *b)
{
	return strncmp(a, b, strlen(a)) == 0;
}
#define HAVE_NEXT (i + 1 < argc)

int main(int argc, char *argv[])
{
#if RUN_TESTS
	printf("Running tests.\n");
	for(int i = 0; i < sizeof(success_tests) / sizeof(*success_tests); i++)
	{
		printf("Running %s\n", success_tests[i]);
		compile(success_tests[i], "a.out");
	}
	return 0;
#else
	const char *input = NULL;
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
		printf("No input file.\n");
	}
	return 0;
#endif
}
