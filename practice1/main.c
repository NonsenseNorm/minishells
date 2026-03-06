#include "types.h"

/* ---- トークン表示 ---- */

static const char	*tok_name(t_tok_type t)
{
	if (t == TOK_WORD)
		return ("WORD");
	if (t == TOK_PIPE)
		return ("PIPE");
	if (t == TOK_REDIRECT_IN)
		return ("REDIR_IN");
	if (t == TOK_REDIRECT_OUT)
		return ("REDIR_OUT");
	if (t == TOK_REDIRECT_APPEND)
		return ("REDIR_APPEND");
	return ("HEREDOC");
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

/* ---- パイプライン表示 ---- */

static const char	*redir_str(t_redirect_type t)
{
	if (t == REDIRECT_IN)
		return ("<");
	if (t == REDIRECT_OUT)
		return (">");
	if (t == REDIRECT_APPEND)
		return (">>");
	return ("<<");
}

static void	print_pipeline(t_pipeline *pl)
{
	int			i;
	int			k;
	t_redirect	*r;

	printf("=== pipeline: %d cmd(s) ===\n", pl->count);
	i = 0;
	while (i < pl->count)
	{
		printf("  cmd[%d] argv:", i);
		k = 0;
		while (pl->cmds[i].argv[k])
		{
			printf(" \"%s\"", pl->cmds[i].argv[k]);
			k++;
		}
		printf("\n");
		r = pl->cmds[i].redirects;
		while (r)
		{
			printf("  cmd[%d] redir: %s \"%s\"\n",
				i, redir_str(r->type), r->target);
			r = r->next;
		}
		i++;
	}
}

/* ---- メインループ ---- */

int	main(void)
{
	char		*line;
	t_token		*tok;
	t_pipeline	pl;

	while (1)
	{
		line = readline("parse> ");
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
		tok = NULL;
		if (lex_line(line, &tok) == 0)
		{
			printf("=== tokens ===\n");
			print_tokens(tok);
			pl.cmds = NULL;
			pl.count = 0;
			if (parse_pipeline(tok, &pl) == 0)
			{
				print_pipeline(&pl);
				free_pipeline(&pl);
			}
		}
		free_tokens(tok);
		free(line);
	}
	return (0);
}
