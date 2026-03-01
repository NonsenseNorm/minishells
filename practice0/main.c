#include "lexer.h"

static const char	*tok_name(t_tok_type t)
{
	if (t == TOK_WORD)            return ("WORD");
	if (t == TOK_PIPE)            return ("PIPE");
	if (t == TOK_REDIRECT_IN)     return ("REDIR_IN");
	if (t == TOK_REDIRECT_OUT)    return ("REDIR_OUT");
	if (t == TOK_REDIRECT_APPEND) return ("REDIR_APPEND");
	if (t == TOK_HEREDOC)         return ("HEREDOC");
	return ("?");
}

static void	print_tokens(t_token *tok)
{
	if (!tok)
	{
		printf("(empty)\n");
		return ;
	}
	while (tok)
	{
		if (tok->value)
			printf("[%s \"%s\"]", tok_name(tok->type), tok->value);
		else
			printf("[%s]", tok_name(tok->type));
		if (tok->next)
			printf(" -> ");
		tok = tok->next;
	}
	printf("\n");
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

int	main(void)
{
	char	*line;
	t_token	*tokens;

	while (1)
	{
		line = readline("lex> ");
		if (!line)
		{
			printf("\n");
			break ;
		}
		if (!*line)
		{
			free(line);
			continue ;
		}
		add_history(line);
		tokens = NULL;
		if (lex_line(line, &tokens) != 0)
			fprintf(stderr, "syntax error\n");
		else
			print_tokens(tokens);
		free_tokens(tokens);
		free(line);
	}
	return (0);
}
