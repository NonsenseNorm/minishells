#include "types.h"

/* ================================================================
   実装済みユーティリティ関数 (変更不要)
   ================================================================ */

int	is_redirect(t_tok_type t)
{
	return (t == TOK_REDIRECT_IN || t == TOK_REDIRECT_OUT
		|| t == TOK_REDIRECT_APPEND || t == TOK_HEREDOC);
}

t_redirect_type	map_redirect(t_tok_type t)
{
	if (t == TOK_REDIRECT_IN)
		return (REDIRECT_IN);
	if (t == TOK_REDIRECT_OUT)
		return (REDIRECT_OUT);
	if (t == TOK_REDIRECT_APPEND)
		return (REDIRECT_APPEND);
	return (REDIRECT_HEREDOC);
}

void	free_pipeline(t_pipeline *pl)
{
	int			i;
	t_redirect	*r;
	t_redirect	*next;

	if (!pl->cmds)
		return ;
	i = 0;
	while (i < pl->count)
	{
		free(pl->cmds[i].argv);
		r = pl->cmds[i].redirects;
		while (r)
		{
			next = r->next;
			if (r->hd_fd >= 0)
				close(r->hd_fd);
			free(r);
			r = next;
		}
		i++;
	}
	free(pl->cmds);
	pl->cmds = NULL;
	pl->count = 0;
}

static int	syntax_err(t_token *tok)
{
	const char	*repr;

	repr = "newline";
	if (tok && tok->type == TOK_PIPE)
		repr = "|";
	fprintf(stderr,
		"minishell: syntax error near unexpected token `%s'\n", repr);
	return (1);
}

static int	add_redirect(t_cmd *cmd, t_token *tok)
{
	t_redirect	*r;
	t_redirect	*cur;

	r = malloc(sizeof(*r));
	if (!r)
		return (1);
	r->type = map_redirect(tok->type);
	r->target = tok->next->value;
	r->hd_fd = -1;    /* ヒアドック: heredoc.c が後で設定する */
	r->next = NULL;
	if (!cmd->redirects)
		cmd->redirects = r;
	else
	{
		cur = cmd->redirects;
		while (cur->next)
			cur = cur->next;
		cur->next = r;
	}
	return (0);
}

static int	count_cmds(t_token *tok)
{
	int	count;

	count = 1;
	while (tok)
	{
		if (tok->type == TOK_PIPE)
			count++;
		tok = tok->next;
	}
	return (count);
}

static int	parse_one(t_token **tokp, t_cmd *cmd)
{
	t_token	*tok;
	int		argc;
	int		k;

	tok = *tokp;
	argc = 0;
	while (tok && tok->type != TOK_PIPE)
	{
		if (tok->type == TOK_WORD)
			argc++;
		else if (is_redirect(tok->type) && tok->next)
			tok = tok->next;
		tok = tok->next;
	}
	cmd->argv = malloc(sizeof(char *) * (size_t)(argc + 1));
	if (!cmd->argv)
		return (1);
	k = 0;
	while (k <= argc)
		cmd->argv[k++] = NULL;
	cmd->redirects = NULL;
	tok = *tokp;
	k = 0;
	while (tok && tok->type != TOK_PIPE)
	{
		if (tok->type == TOK_WORD)
			cmd->argv[k++] = tok->value;
		else
		{
			if (!tok->next || tok->next->type != TOK_WORD)
				return (syntax_err(tok->next));
			if (add_redirect(cmd, tok) != 0)
				return (1);
			tok = tok->next;
		}
		tok = tok->next;
	}
	*tokp = tok;
	return (0);
}

static int	parse_cmds(t_token *tok, t_pipeline *pl)
{
	int	i;

	i = 0;
	while (i < pl->count)
	{
		if (parse_one(&tok, &pl->cmds[i]) != 0)
			return (1);
		if (tok && tok->type == TOK_PIPE)
			tok = tok->next;
		i++;
	}
	if (tok)
		return (syntax_err(tok));
	return (0);
}

int	parse_pipeline(t_token *tok, t_pipeline *pl)
{
	pl->cmds = NULL;
	pl->count = 0;
	if (!tok || tok->type == TOK_PIPE)
		return (syntax_err(tok));
	pl->count = count_cmds(tok);
	if (pl->count == 0)
		return (1);
	pl->cmds = malloc(sizeof(t_cmd) * (size_t)pl->count);
	if (!pl->cmds)
		return (1);
	if (parse_cmds(tok, pl) != 0)
	{
		free_pipeline(pl);
		return (1);
	}
	return (0);
}
