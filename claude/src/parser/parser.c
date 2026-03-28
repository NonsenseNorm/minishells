/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/01/01 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser_internal.h"
#include "../mem/mem.h"
#include "../core/core.h"

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

static int	syntax_error_unexpected(t_shell *sh, t_token *tok)
{
	const char	*repr;

	repr = "newline";
	if (tok && tok->type == TOK_PIPE)
		repr = "|";
	ms_err("minishell: syntax error near unexpected token `");
	ms_err(repr);
	ms_err("'\n");
	sh->exit_code = 2;
	return (1);
}

int	parse_pipeline(t_shell *sh, t_mem *mem, t_token *tok, t_pipeline *pl)
{
	if (!tok || tok->type == TOK_PIPE)
		return (syntax_error_unexpected(sh, tok));
	pl->count = count_cmds(tok);
	pl->cmds = ms_alloc(mem, sizeof(t_cmd) * pl->count);
	if (!pl->cmds)
		return (1);
	if (parse_cmds(sh, mem, tok, pl) != 0)
		return (sh->exit_code = 2, 1);
	return (0);
}
