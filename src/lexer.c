#include "lexer.h"
#include <ctype.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

static int g_line_count;
static int g_token_count;
//WARNING: if you change this, you must chage
//the keyword enum.
static const char *kw_strings[] =
{
	"break",
	"case",
	"continue",
	"default",
	"do",
	"else",
	"from",
	"function",
	"if",
	"import",
	"namespace",
	"return",
	"struct",
	"typedef",
	"void",
	"while",
};

static char *read_in(FILE *fp)
{
	fseek(fp, 0, SEEK_END);
	size_t size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *out = (char*)malloc(size + 1);
	fread(out, 1, size, fp);
	out[size] = '\0';
	return out;
}

static bool isseperator(char in)
{
	return isspace(in) || in == ',' || in == '.' || in == ';' || in == '(' || in == ')' || in == ':';
}

static bool isop(char in)
{
	const char *opstring = "+-*/<>=?~!%^&|";
	const char *cur      = opstring;
	char c;
	while( (c = *cur++) )
	{
		if(in == c) return 1;
	}
	return 0;
}

#define X(name) case TOK_ ## name : printf(#name "\n"); break;
void print_token(token_t *token)
{
	printf("%d ", token->line);
	switch(token->type)
	{
		case TOK_INTEGER:
			printf("INT %d\n", token->integer);
			break;
		case TOK_FLOAT:
			printf("FLT %f\n", token->flt);
			break;
		case TOK_ID:
			printf("ID %s\n", token->string);
			break;
		case TOK_STRING:
			printf("STRING: \"%s\"\n", token->string);
			break;
		case TOK_OP:
			printf("OP: %c\n", token->op);
			break;
		case TOK_KEYWORD:
			printf("KW: %s\n", kw_strings[token->kw]);
			break;
		X(ARROW);
		X(LPAREN);
		X(RPAREN);
		X(LBRACE);
		X(RBRACE);
		X(SEMICOLON);
		X(COLON);
		X(PERIOD);
		X(NEWLINE);
		X(COMMA);
		X(NONE);
		default:
			printf("Error Unknown type\n");
			exit(-1);
	}
}
#undef X

static void new_token(token_t **token)
{
#if VERBOSE_LEXER
	print_token(&(*token)[g_token_count]);
#endif
	g_token_count++;
	*token = (token_t*) realloc(*token, sizeof(token_t) * (g_token_count + 1));
	(*token)[g_token_count].type = TOK_NONE;
	(*token)[g_token_count].line = g_line_count;
}

void unexpected_end_error(void) __attribute__ ((noreturn));
void invalid_input_error(void) __attribute__ ((noreturn));

void unexpected_end_error(void)
{
	fprintf(stderr, "Unexpected end of file.\n");
	exit(-1);
}

void invalid_input_error(void)
{
	fprintf(stderr, "Invalid input.\n");
	exit(-1);
}

static token_t *read_float(char **pos, token_t *in)
{
	in[g_token_count].type = TOK_FLOAT;
	in[g_token_count].flt  = strtod(*pos, pos);

	if(**pos == 'f') (*pos)++;

	new_token(&in);
	return in;
}

static token_t *read_int(char **pos_, token_t *in)
{
	char c;
	char *pos = *pos_;
	short idx = 0;

	while((c = pos[idx++]) && isdigit(c));

	if(c == '\0')
	{
		unexpected_end_error();
	}
	else if(c == '.' || c == 'e' || c == 'E'  || c == 'f')
	{
		read_float(pos_, in);
	}
	else if(isseperator(c) || c == 'x')
	{
		in[g_token_count].integer = strtol(*pos_, pos_, 0);
		in[g_token_count].type    = TOK_INTEGER;
		new_token(&in);
	}
	else
	{
		invalid_input_error();
	}
	return in;
}

static int is_keyword(const char *string)
{
	for(int i = 0; i < NUM_KEYWORDS; i++)
	{
		if(strcmp(string, kw_strings[i]) == 0) return i;
	}
	return -1;
}

static token_t *read_id(char **pos_, token_t *in)
{
	char c;
	char *pos = *pos_;
	while( (c = *pos++) )
	{
		if(isseperator(c))
		{
			char *string = (char*) calloc(1, (size_t)(pos - *pos));
			in[g_token_count].string = string;
			in[g_token_count].type   = TOK_ID;
			memcpy(string, *pos_, pos - *pos_ - 1);
			int kw = is_keyword(string);
			if(is_keyword(string) >= 0)
			{
				free(string);
				in[g_token_count].type = TOK_KEYWORD;
				in[g_token_count].kw   = (enum KW) kw;
			}
			new_token(&in);
			*pos_ = pos - 1;
			return in;
		}
	}
	*pos_ = pos;
	return in;
}

static void invalid_token_error() __attribute__((noreturn));
static void invalid_token_error()
{
	fprintf(stderr, "Invalid token found.\n");
	exit(-1);
}

static token_t *read_single(char **pos, token_t *in)
{
	enum TOK token;
	switch(**pos)
	{
		case '(': token = TOK_LPAREN;    break;
		case ')': token = TOK_RPAREN;    break;
		case '{': token = TOK_LBRACE;    break;
		case '}': token = TOK_RBRACE;    break;
		case ';': token = TOK_SEMICOLON; break;
		case ':': token = TOK_COLON; break;
		case ',': token = TOK_COMMA; break;
		case '.': token = TOK_PERIOD; break;
		default:
			invalid_token_error();
	}
	in[g_token_count].type = token;
	(*pos)++;
	new_token(&in);
	return in;
}

// Check the two next chars, if they match the input params
// a and b, assign c as the operation type
#define X(a, b, c)\
	else if((*pos)[0] == (a) && (*pos)[1] == (b))\
	{\
		in[g_token_count].type = TOK_OP;\
		in[g_token_count].op   = c;\
		*pos += 2;\
	}
static token_t *read_op(char **pos, token_t *in)
{
	if((*pos)[0] == '-' && (*pos)[1] == '>')
	{
		in[g_token_count].type = TOK_ARROW;
		*pos += 2;
	}
	X('=', '=', OP_EQ)
	X('!', '=', OP_NEQ)
	X('<', '=', OP_LTE)
	X('>', '=', OP_GTE)
	else if((*pos)[0] == '=')
	{
		in[g_token_count].type = TOK_ASSIGN;
		*pos += 1;
	}
	else
	{
		in[g_token_count].type = TOK_OP;
		in[g_token_count].op   = (enum OP) **pos;
		(*pos)++;
	}
	new_token(&in);
	return in;
}
#undef X

static token_t *read_string(char **pos_, token_t *in)
{
	char c, *pos = *pos_ + 1;
	*pos_ = pos;
	bool escape = false;
	int count = 0;
	while( (c = *pos++) && (escape || c != '"'))
	{
		count++;
		escape = (c == '\\');
	}

	if(!c) unexpected_end_error();

	in[g_token_count].type          = TOK_STRING;
	in[g_token_count].string        = (char*) malloc( count + 1);
	in[g_token_count].string[count] = '\0';
	memcpy(in[g_token_count].string, *pos_, count);

	*pos_ = pos;
	new_token(&in);
	return in;
}

token_t *tokenise(FILE *fp)
{
	char    *file = read_in(fp);
	token_t *out  = MALLOC_T(token_t);
	out->type     = TOK_NONE;
	out->line     = 1;

	printf("%s\n----------------\n", file);

	g_line_count  = 1;
	g_token_count = 0;

	char *pos = file;
	char cur;
	while(*pos)
	{
		if(isdigit(*pos))
		{
			out = read_int(&pos, out);
		}
		else if(*pos == '(' || *pos == ')' ||
				*pos == '{' || *pos == '}' ||
				*pos == ';' || *pos == ':' ||
				*pos == ',' || *pos == '.')
		{
			out = read_single(&pos, out);
		}
		else if(*pos == '\n')
		{
			out[g_token_count].type = TOK_NEWLINE;
			while(*pos == '\n')
			{
				pos++;
				g_line_count++;
			}
			new_token(&out);
		}
		else if(isspace(*pos))
		{
			pos++;
		}
		else if(isalpha(*pos))
		{
			out = read_id(&pos, out);
		}
		else if(isop(*pos))
		{
			out = read_op(&pos, out);
		}
		else if(*pos == '"')
		{
			out = read_string(&pos, out);
		}
		else
		{
			fprintf(stderr, "Error %c\n", *pos);
			exit(-1);
		}
	}
	return out;
}
