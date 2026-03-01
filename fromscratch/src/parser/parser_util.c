/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_util.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/02/24 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../core/minishell.h"

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
