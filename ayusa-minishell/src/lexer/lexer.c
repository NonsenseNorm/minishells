/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:11:43 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/21 20:28:24 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static t_token	*consume_operator(char **line_ptr);
static t_token	*consume_word(char **line_ptr);

// skip_spaces -> consume_operator || consume_word -> token_add_back
t_token	*tokenize(char *line)
{
	t_token	*head;
	t_token	*new_nd;

	head = NULL;
	while (line)
	{
		skip_spaces(&line);
		if (*line == '\0')
			break ;
		if (is_operator(*line))
			new_nd = consume_operator(&line);
		else
			new_nd = consume_word(&line);
		if (new_nd == NULL)
		{
			free_tokens(head);
			return (NULL);
		}
		token_add_back(&head, new_nd);
	}
	return (head);
}

// |, <, >, &, (, ) - if &' only return
static t_token	*consume_operator(char **line_ptr)
{
	t_token_type	type;
	int				len;

	len = 1;
	if (classify_operator(*line_ptr, &type, &len) == -1)
	{
		ft_putendl_fd(
			"minishell: syntax error near unexpected token `&'", STDERR_FILENO);
		return (NULL);
	}
	*line_ptr += len;
	return (new_token(type, ft_substr(*line_ptr - len, 0, len)));
}

// nomal word
static t_token	*consume_word(char **line_ptr)
{
	char	*start;
	char	*value;
	int		in_squote;
	int		in_dquote;

	start = *line_ptr;
	in_squote = 0;
	in_dquote = 0;
	while (**line_ptr != '\0')
	{
		if (!in_squote && !in_dquote && is_spaces_operator(**line_ptr))
			break ;
		if (**line_ptr == '\'' && !in_dquote)
			in_squote = !in_squote;
		else if (**line_ptr == '"' && !in_squote)
			in_dquote = !in_dquote;
		(*line_ptr)++;
	}
	if (in_squote || in_dquote)
	{
		ft_putendl_fd("minishell: unclosed quote error", STDERR_FILENO);
		return (NULL);
	}
	value = ft_substr(start, 0, *line_ptr - start);
	return (new_token(TK_WORD, value));
}

void	free_tokens(t_token *tokens)
{
	t_token	*tmp;

	while (tokens)
	{
		tmp = tokens->next;
		if (tokens->value)
		{
			free(tokens->value);
			tokens->value = NULL;
		}
		free(tokens);
		tokens = tmp;
	}
}
