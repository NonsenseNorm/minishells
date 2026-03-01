/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_cmd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/02/24 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../core/minishell.h"

static int	syntax_err(t_shell *sh, t_token *tok)
{
	const char	*s;

	if (!tok)
		s = "newline";
	else if (tok->type == TOK_WORD)
		s = tok->value;
	else if (tok->type == TOK_PIPE)
		s = "|";
	else if (tok->type == TOK_REDIRECT_IN)
		s = "<";
	else if (tok->type == TOK_REDIRECT_OUT)
		s = ">";
	else if (tok->type == TOK_REDIRECT_APPEND)
		s = ">>";
	else
		s = "<<";
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	ft_putstr_fd((char *)s, 2);
	ft_putstr_fd("'\n", 2);
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
	r->type = REDIRECT_HEREDOC;
	if (tok->type == TOK_REDIRECT_IN)
		r->type = REDIRECT_IN;
	else if (tok->type == TOK_REDIRECT_OUT)
		r->type = REDIRECT_OUT;
	else if (tok->type == TOK_REDIRECT_APPEND)
		r->type = REDIRECT_APPEND;
	r->target = tok->next->value;
	r->quoted = has_quote(tok->next->value);
	r->next = NULL;
	cur = cmd->redirects;
	while (cur && cur->next)
		cur = cur->next;
	if (!cur)
		cmd->redirects = r;
	else
		cur->next = r;
	return (0);
}

static int	parse_one_fill(t_shell *sh, t_token **tokp, t_cmd *cmd, int argc)
{
	t_token	*tok;
	int		k;

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

static int	parse_one(t_shell *sh, t_token **tokp, t_cmd *cmd)
{
	t_token	*tok;
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
	return (parse_one_fill(sh, tokp, cmd, argc));
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
