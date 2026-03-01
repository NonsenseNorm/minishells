/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claude <claude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/01/01 00:00:00 by claude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../core/ms.h"

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
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	ft_putstr_fd((char *)repr, 2);
	ft_putstr_fd("'\n", 2);
	sh->exit_code = 2;
	return (1);
}

int	parse_pipeline(t_shell *sh, t_token *tok, t_pipeline *pl)
{
	if (!tok || tok->type == TOK_PIPE)
		return (syntax_error_unexpected(sh, tok));
	pl->count = count_cmds(tok);
	pl->cmds = ms_alloc(&sh->mem, sizeof(t_cmd) * pl->count);
	if (!pl->cmds)
		return (1);
	if (parse_cmds(sh, tok, pl) != 0)
		return (sh->exit_code = 2, 1);
	return (0);
}
