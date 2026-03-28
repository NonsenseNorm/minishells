/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/03/26 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer_internal.h"
#include "../mem/mem.h"
#include "../core/core.h"

static int	push_tok(t_mem *mem, t_tok_type type, char *v, t_token **lst)
{
	t_token	*t;
	t_token	*cur;

	t = ms_alloc(mem, sizeof(*t));
	if (!t)
		return (1);
	t->type = type;
	t->value = v;
	t->next = NULL;
	if (!*lst)
		return (*lst = t, 0);
	cur = *lst;
	while (cur->next)
		cur = cur->next;
	cur->next = t;
	return (0);
}

static int	lex_op(t_mem *mem, const char *s, int *i, t_token **out)
{
	t_tok_type	type;

	type = TOK_PIPE;
	if (s[*i] == '<' && s[*i + 1] == '<')
		type = TOK_HEREDOC;
	else if (s[*i] == '>' && s[*i + 1] == '>')
		type = TOK_REDIRECT_APPEND;
	else if (s[*i] == '<')
		type = TOK_REDIRECT_IN;
	else if (s[*i] == '>')
		type = TOK_REDIRECT_OUT;
	if (type == TOK_HEREDOC || type == TOK_REDIRECT_APPEND)
		*i += 2;
	else
		*i += 1;
	return (push_tok(mem, type, NULL, out));
}

static int	lex_word(t_mem *mem, const char *line, int *i, t_token **out)
{
	int		st;
	int		q;
	char	*word;

	st = *i;
	q = lex_word_end(line, i);
	if (q != 0)
		return (q);
	word = ms_strndup(mem, line + st, *i - st);
	push_tok(mem, TOK_WORD, word, out);
	return (0);
}

static int	syntax_error_quote(t_shell *sh, char quote)
{
	ms_err("minishell: unexpected EOF while looking for matching `");
	write(2, &quote, 1);
	ms_err("'\n");
	ms_err("minishell: syntax error: unexpected end of file\n");
	sh->exit_code = 2;
	return (1);
}

int	lex_line(t_shell *sh, t_mem *mem, const char *line, t_token **out)
{
	int	i;
	int	q;

	i = 0;
	*out = NULL;
	while (line[i] && line[i] != '\n')
	{
		while (line[i] == ' ' || line[i] == '\t')
			i++;
		if (!line[i] || line[i] == '\n')
			break ;
		if (ft_strchr("|<>", line[i]))
			lex_op(mem, line, &i, out);
		else
		{
			q = lex_word(mem, line, &i, out);
			if (q != 0)
				return (syntax_error_quote(sh, (char)q));
		}
	}
	return (0);
}
