/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_cmd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claude <claude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/01/01 00:00:00 by claude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../core/ms.h"

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

static int	syntax_err(t_shell *sh, t_token *tok)
{
	ms_err("minishell: syntax error near unexpected token `");
	ms_err(tok_repr(tok));
	ms_err("'\n");
	sh->exit_code = 2;
	return (1);
}

static int	add_redirect(t_shell *sh, t_cmd *cmd, t_token *tok)
{
	t_redirect	*r;
	t_redirect	*cur;

	r = ms_alloc(&sh->mem, sizeof(*r));
	if (!r)
		return (1);
	r->type = map_redirect(tok->type);
	r->target = tok->next->value;
	r->quoted = has_quote(tok->next->value);
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
		return (syntax_err(sh, *tokp));
	argc = 0;
	tok = *tokp;
	while (tok && tok->type != TOK_PIPE)
	{
		if (tok->type == TOK_WORD)
			argc++;
		else if (is_redirect(tok->type) && tok->next)
			tok = tok->next;
		tok = tok->next;
	}
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
			return (syntax_err(sh, tok->next));
		else if (add_redirect(sh, cmd, tok) != 0)
			return (1);
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
			return (syntax_err(sh, NULL));
		i++;
	}
	if (tok)
		return (syntax_err(sh, tok));
	return (0);
}
