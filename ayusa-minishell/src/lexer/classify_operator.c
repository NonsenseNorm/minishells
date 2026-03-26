/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   classify_operator.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 18:00:00 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/18 18:00:00 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static int	classify_redir_in(char *s, t_token_type *type, int *len);
static int	classify_redir_out(char *s, t_token_type *type, int *len);
static int	classify_pipe_and(char *s, t_token_type *type, int *len);

int	classify_operator(char *s, t_token_type *type, int *len)
{
	int	ret;

	if (classify_redir_in(s, type, len))
		return (0);
	if (classify_redir_out(s, type, len))
		return (0);
	ret = classify_pipe_and(s, type, len);
	if (ret == -1)
		return (-1);
	if (ret == 1)
		return (0);
	if (s[0] == '(')
	{
		*type = TK_LPAREN;
		*len = 1;
	}
	else
	{
		*type = TK_RPAREN;
		*len = 1;
	}
	return (0);
}

static int	classify_redir_in(char *s, t_token_type *type, int *len)
{
	if (s[0] == '<' && s[1] == '<')
	{
		*type = TK_HEREDOC;
		*len = 2;
	}
	else if (s[0] == '<' && s[1] == '>')
	{
		*type = TK_REDIR_RDWR;
		*len = 2;
	}
	else if (s[0] == '<')
	{
		*type = TK_REDIR_IN;
		*len = 1;
	}
	else
		return (0);
	return (1);
}

static int	classify_redir_out(char *s, t_token_type *type, int *len)
{
	if (s[0] == '>' && s[1] == '>')
	{
		*type = TK_REDIR_APPEND;
		*len = 2;
	}
	else if (s[0] == '>')
	{
		*type = TK_REDIR_OUT;
		*len = 1;
	}
	else
		return (0);
	return (1);
}

static int	classify_pipe_and(char *s, t_token_type *type, int *len)
{
	if (s[0] == '|' && s[1] == '|')
	{
		*type = TK_OR;
		*len = 2;
	}
	else if (s[0] == '|')
	{
		*type = TK_PIPE;
		*len = 1;
	}
	else if (s[0] == '&' && s[1] == '&')
	{
		*type = TK_AND;
		*len = 2;
	}
	else if (s[0] == '&')
		return (-1);
	else
		return (0);
	return (1);
}
