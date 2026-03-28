/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_cmd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/03/26 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser_internal.h"
#include "../mem/mem.h"

static int	add_redirect(t_mem *mem, t_cmd *cmd, t_token *tok)
{
	t_redirect	*r;
	t_redirect	*cur;

	r = ms_alloc(mem, sizeof(*r));
	if (!r)
		return (1);
	r->type = (t_redirect_type)(tok->type - TOK_REDIRECT_IN);
	r->target = ms_strdup(mem, tok->next->value);
	r->quoted = has_quote(tok->next->value);
	r->fd = -1;
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

static int	count_argc(t_token *tok)
{
	int	argc;

	argc = 0;
	while (tok && tok->type != TOK_PIPE)
	{
		if (tok->type == TOK_WORD)
			argc++;
		else if (is_redirect(tok->type) && tok->next)
			tok = tok->next;
		tok = tok->next;
	}
	return (argc);
}

static int	fill_cmd(t_shell *sh, t_mem *mem, t_cmd *cmd, t_token **tokp)
{
	t_token	*cur;
	int		k;

	cur = *tokp;
	k = 0;
	while (cur && cur->type != TOK_PIPE)
	{
		if (cur->type == TOK_WORD)
			cmd->argv[k++] = ms_strdup(mem, cur->value);
		else if (!cur->next || cur->next->type != TOK_WORD)
			return (syntax_err(sh, cur->next));
		else if (add_redirect(mem, cmd, cur) != 0)
			return (1);
		if (is_redirect(cur->type))
			cur = cur->next;
		cur = cur->next;
	}
	*tokp = cur;
	return (0);
}

static int	parse_one(t_shell *sh, t_mem *mem, t_token **tokp, t_cmd *cmd)
{
	int	n;

	if (*tokp && (*tokp)->type == TOK_PIPE)
		return (syntax_err(sh, *tokp));
	n = count_argc(*tokp);
	cmd->argv = ms_alloc(mem, sizeof(char *) * (n + 1));
	if (!cmd->argv)
		return (1);
	cmd->argv[n] = NULL;
	cmd->redirects = NULL;
	return (fill_cmd(sh, mem, cmd, tokp));
}

int	parse_cmds(t_shell *sh, t_mem *mem, t_token *tok, t_pipeline *pl)
{
	int	i;

	i = 0;
	while (i < pl->count)
	{
		if (parse_one(sh, mem, &tok, &pl->cmds[i]) != 0)
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
