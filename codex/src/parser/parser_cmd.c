#include "../core/ms.h"

static int	is_redirect(t_tok_type t)
{
	return (t == TOK_REDIRECT_IN || t == TOK_REDIRECT_OUT
		|| t == TOK_REDIRECT_APPEND || t == TOK_HEREDOC);
}

static const char	*tok_repr(t_token *tok)
{
	if (!tok)
		return ("newline");
	if (tok->type == TOK_WORD)
		return (tok->value);
	if (tok->type == TOK_PIPE)
		return ("|");
	if (tok->type == TOK_REDIRECT_IN)
		return ("<");
	if (tok->type == TOK_REDIRECT_OUT)
		return (">");
	if (tok->type == TOK_REDIRECT_APPEND)
		return (">>");
	return ("<<");
}

static int	syntax_error_unexpected(t_shell *sh, t_token *tok)
{
	fprintf(stderr, "minishell: syntax error near unexpected token `%s'\n",
		tok_repr(tok));
	sh->exit_code = 2;
	return (1);
}

static int	count_args(t_token *tok)
{
	int	count;

	count = 0;
	while (tok && tok->type != TOK_PIPE)
	{
		if (tok->type == TOK_WORD)
			count++;
		else if (is_redirect(tok->type) && tok->next)
			tok = tok->next;
		tok = tok->next;
	}
	return (count);
}

static t_redirect_type	map_redirect(t_tok_type t)
{
	if (t == TOK_REDIRECT_IN)
		return (REDIRECT_IN);
	if (t == TOK_REDIRECT_OUT)
		return (REDIRECT_OUT);
	if (t == TOK_REDIRECT_APPEND)
		return (REDIRECT_APPEND);
	return (REDIRECT_HEREDOC);
}

static int	add_redirect(t_shell *sh, t_cmd *cmd, t_tok_type t, t_token *w)
{
	t_redirect	*r;
	t_redirect	*cur;

	r = ms_alloc(&sh->mem, sizeof(*r));
	if (!r)
		return (1);
	r->type = map_redirect(t);
	r->target = w->value;
	r->quoted = has_quote(w->value);
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

static int	parse_one(t_shell *sh, t_token **tokp, t_cmd *cmd)
{
	t_token	*tok;
	int		k;
	int		argc;

	if (*tokp && (*tokp)->type == TOK_PIPE)
		return (syntax_error_unexpected(sh, *tokp));
	argc = count_args(*tokp);
	cmd->argv = ms_alloc(&sh->mem, sizeof(char *) * (argc + 1));
	if (!cmd->argv)
		return (1);
	ft_bzero(cmd->argv, sizeof(char *) * (argc + 1));
	cmd->redirects = NULL;
	tok = *tokp;
	k = 0;
	while (tok && tok->type != TOK_PIPE)
	{
		if (tok->type == TOK_WORD)
			cmd->argv[k++] = tok->value;
		else if (!tok->next || tok->next->type != TOK_WORD)
			return (syntax_error_unexpected(sh, tok->next));
		else
		{
			if (add_redirect(sh, cmd, tok->type, tok->next) != 0)
				return (1);
		}
		if (is_redirect(tok->type))
			tok = tok->next;
		tok = tok->next;
	}
	*tokp = tok;
	return (0);
}

int	parse_cmds(t_shell *sh, t_token *tok, t_pipeline *pl)
{
	int	i;

	i = 0;
	while (i < pl->count)
	{
		if (parse_one(sh, &tok, &pl->cmds[i]) != 0)
			return (1);
		if (tok && tok->type == TOK_PIPE)
			tok = tok->next;
		if (i + 1 < pl->count && !tok)
			return (syntax_error_unexpected(sh, NULL));
		i++;
	}
	if (tok)
		return (syntax_error_unexpected(sh, tok));
	return (0);
}
