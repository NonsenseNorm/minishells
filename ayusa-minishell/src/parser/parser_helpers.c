/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_helpers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 16:36:31 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/21 16:29:26 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static char	**move_args(char **old_args, int *count);

t_node	*new_node(t_node_type type, t_node *left, t_node *right)
{
	t_node	*node;

	node = (t_node *)malloc(sizeof(t_node));
	if (!node)
		return (NULL);
	node->type = type;
	node->left = left;
	node->right = right;
	node->args = NULL;
	node->redirects = NULL;
	return (node);
}

int	is_redirect(t_token_type type)
{
	if (type == TK_REDIR_IN || type == TK_REDIR_OUT
		|| type == TK_REDIR_APPEND || type == TK_HEREDOC
		|| type == TK_REDIR_RDWR)
		return (1);
	return (0);
}

//　Reallocate memory for `args` and add it
void	append_arg(t_node *node, char *value)
{
	char	**new_args;
	char	*dup;
	int		i;

	dup = ft_strdup(value);
	if (!dup)
		return ;
	new_args = move_args(node->args, &i);
	if (!new_args)
	{
		free(dup);
		return ;
	}
	new_args[i] = dup;
	new_args[i + 1] = NULL;
	node->args = new_args;
}

static char	**move_args(char **old_args, int *count)
{
	char	**new_args;
	int		i;

	i = 0;
	if (old_args)
	{
		while (old_args[i])
			i++;
	}
	*count = i;
	new_args = (char **)malloc(sizeof(char *) * (i + 2));
	if (!new_args)
		return (NULL);
	i = 0;
	if (old_args)
	{
		while (old_args[i])
		{
			new_args[i] = old_args[i];
			i++;
		}
		free(old_args);
	}
	return (new_args);
}
