/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/02/24 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../core/minishell.h"

/*
** Tokenises `line` into a linked list of t_token written to *out.
** Returns 0 on success, 1 on syntax error.
*/
int	lex_line(t_shell *sh, const char *line, t_token **out)
{
	(void)sh;
	(void)line;
	*out = NULL;
	return (0);
}
