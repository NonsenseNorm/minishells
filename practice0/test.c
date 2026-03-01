#include "lexer.h"

#define GREEN  "\033[32m"
#define RED    "\033[31m"
#define RESET  "\033[0m"

/* Expected token: type == -1 is the end sentinel */
typedef struct {
	int			type;
	const char	*value;
}	t_exp;

typedef struct {
	const char	*name;
	const char	*input;
	int			expect_err;
	t_exp		exp[10];
}	t_case;

#define W(s)  {TOK_WORD,            (s)}
#define P     {TOK_PIPE,            NULL}
#define RI    {TOK_REDIRECT_IN,     NULL}
#define RO    {TOK_REDIRECT_OUT,    NULL}
#define RA    {TOK_REDIRECT_APPEND, NULL}
#define HD    {TOK_HEREDOC,         NULL}
#define END   {-1,                  NULL}

static const t_case	g_tests[] = {
	/* basic words */
	{"empty string",          "",                       0, {END}},
	{"single word",           "echo",                   0, {W("echo"), END}},
	{"two words",             "echo hello",             0, {W("echo"), W("hello"), END}},
	{"extra spaces",          "  echo  hi  ",           0, {W("echo"), W("hi"), END}},
	/* operators alone */
	{"pipe",                  "|",                      0, {P, END}},
	{"redir in",              "<",                      0, {RI, END}},
	{"redir out",             ">",                      0, {RO, END}},
	{"redir append",          ">>",                     0, {RA, END}},
	{"heredoc",               "<<",                     0, {HD, END}},
	/* mixed: words and operators */
	{"cmd | cmd",             "echo | cat",             0, {W("echo"), P, W("cat"), END}},
	{"cmd > file",            "echo > out",             0, {W("echo"), RO, W("out"), END}},
	{"cmd < file",            "cat < in",               0, {W("cat"), RI, W("in"), END}},
	{"cmd >> file",           "echo >> file",           0, {W("echo"), RA, W("file"), END}},
	{"cmd << delim",          "cat << EOF",             0, {W("cat"), HD, W("EOF"), END}},
	{"a | b | c",             "a | b | c",              0, {W("a"), P, W("b"), P, W("c"), END}},
	{"< in > out",            "< in > out",             0, {RI, W("in"), RO, W("out"), END}},
	/* no spaces around operators */
	{"pipe no space",         "echo|cat",               0, {W("echo"), P, W("cat"), END}},
	{"redir in no space",     "cat<in",                 0, {W("cat"), RI, W("in"), END}},
	{"redir out no space",    "echo>out",               0, {W("echo"), RO, W("out"), END}},
	{"redir append no space", "echo>>out",              0, {W("echo"), RA, W("out"), END}},
	/* single quotes */
	{"single quoted",         "echo 'hello world'",     0, {W("echo"), W("'hello world'"), END}},
	{"empty single quotes",   "''",                     0, {W("''"), END}},
	{"quote glued to word",   "echo 'hi'world",         0, {W("echo"), W("'hi'world"), END}},
	{"pipe inside quotes",    "'hello|world'",          0, {W("'hello|world'"), END}},
	/* double quotes */
	{"double quoted",         "echo \"hello world\"",   0, {W("echo"), W("\"hello world\""), END}},
	{"empty double quotes",   "\"\"",                   0, {W("\"\""), END}},
	{"dquote with pipe",      "\"a|b\" | cat",          0, {W("\"a|b\""), P, W("cat"), END}},
	/* errors */
	{"unclosed single quote", "echo 'hello",            1, {END}},
	{"unclosed double quote", "echo \"hello",           1, {END}},
};

static const char	*tok_name(int t)
{
	if (t == TOK_WORD)            return ("WORD");
	if (t == TOK_PIPE)            return ("PIPE");
	if (t == TOK_REDIRECT_IN)     return ("REDIR_IN");
	if (t == TOK_REDIRECT_OUT)    return ("REDIR_OUT");
	if (t == TOK_REDIRECT_APPEND) return ("REDIR_APPEND");
	if (t == TOK_HEREDOC)         return ("HEREDOC");
	return ("?");
}

static void	free_tokens(t_token *tok)
{
	t_token	*next;

	while (tok)
	{
		next = tok->next;
		free(tok->value);
		free(tok);
		tok = next;
	}
}

static int	check_tokens(t_token *tok, const t_exp *exp)
{
	int	i;

	i = 0;
	while (exp[i].type != -1 && tok)
	{
		if ((int)tok->type != exp[i].type)
			return (0);
		if (!exp[i].value != !tok->value)
			return (0);
		if (exp[i].value && strcmp(tok->value, exp[i].value) != 0)
			return (0);
		tok = tok->next;
		i++;
	}
	return (exp[i].type == -1 && tok == NULL);
}

static void	print_exp(const t_exp *exp)
{
	int	i;

	i = 0;
	while (exp[i].type != -1)
	{
		if (i)
			printf(" -> ");
		if (exp[i].value)
			printf("[%s \"%s\"]", tok_name(exp[i].type), exp[i].value);
		else
			printf("[%s]", tok_name(exp[i].type));
		i++;
	}
	if (i == 0)
		printf("(empty)");
	printf("\n");
}

static void	print_actual(t_token *tok)
{
	int	first;

	first = 1;
	if (!tok)
	{
		printf("(empty)\n");
		return ;
	}
	while (tok)
	{
		if (!first)
			printf(" -> ");
		first = 0;
		if (tok->value)
			printf("[%s \"%s\"]", tok_name(tok->type), tok->value);
		else
			printf("[%s]", tok_name(tok->type));
		tok = tok->next;
	}
	printf("\n");
}

static int	run_test(const t_case *tc)
{
	t_token	*tokens;
	int		ret;
	int		ok;

	tokens = NULL;
	ret = lex_line(tc->input, &tokens);
	if (tc->expect_err)
		ok = (ret != 0);
	else
		ok = (ret == 0) && check_tokens(tokens, tc->exp);
	if (ok)
		printf(GREEN "[PASS]" RESET " %s\n", tc->name);
	else
	{
		printf(RED "[FAIL]" RESET " %s\n", tc->name);
		if (tc->expect_err)
		{
			printf("       expected: error return\n");
			printf("       actual:   ret=%d, tokens=", ret);
			print_actual(tokens);
		}
		else
		{
			printf("       expected: ");
			print_exp(tc->exp);
			printf("       actual:   ");
			print_actual(tokens);
		}
	}
	free_tokens(tokens);
	return (ok);
}

int	main(void)
{
	int	i;
	int	n;
	int	pass;

	n = (int)(sizeof(g_tests) / sizeof(g_tests[0]));
	pass = 0;
	i = 0;
	while (i < n)
	{
		pass += run_test(&g_tests[i]);
		i++;
	}
	printf("\n%d/%d tests passed\n", pass, n);
	return (pass == n ? 0 : 1);
}
