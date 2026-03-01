/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_util.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claude <claude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/01/01 00:00:00 by claude           ###   ########.fr       */
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
