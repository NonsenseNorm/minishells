/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_util.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claude <claude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/03/26 00:00:00 by claude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../core/ms.h"

int	has_quote(const char *s)
{
	while (*s)
	{
		if (*s == '\'' || *s == '"')
			return (1);
		s++;
	}
	return (0);
}

int	is_redirect(t_tok_type t)
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

int	syntax_err(t_shell *sh, t_token *tok)
{
	ms_err("minishell: syntax error near unexpected token `");
	ms_err(tok_repr(tok));
	ms_err("'\n");
	sh->exit_code = 2;
	return (1);
}
