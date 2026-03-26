/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_helpers.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 16:17:52 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/18 15:17:43 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

void	skip_spaces(char **line_ptr)
{
	while (**line_ptr == ' ' || **line_ptr == '\t')
		(*line_ptr)++;
}

t_token	*new_token(t_token_type type, char *value)
{
	t_token	*node;

	node = (t_token *)malloc(sizeof(t_token));
	if (!node)
		return (NULL);
	node->type = type;
	node->value = value;
	node->next = NULL;
	return (node);
}

void	token_add_back(t_token **head, t_token *new_node)
{
	t_token	*current;

	if (!head || !new_node)
		return ;
	if (*head == NULL)
	{
		*head = new_node;
		return ;
	}
	current = *head;
	while (current->next != NULL)
		current = current->next;
	current->next = new_node;
}

int	is_operator(char c)
{
	if (c == '|' || c == '<' || c == '>'
		|| c == '&' || c == '(' || c == ')')
		return (1);
	return (0);
}

int	is_spaces_operator(char c)
{
	if (c == ' ' || c == '\t' || c == '\n')
		return (1);
	if (is_operator(c))
		return (1);
	return (0);
}
