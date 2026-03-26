/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_args.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 15:54:59 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/21 22:10:35 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/minishell.h"

static int	handle_expand_arg(t_node *node, int *i, char *expanded,
				int has_wildcard);
static int	expand_arg_wildcard(t_node *node, int i, char *expanded);
static int	remove_empty_arg(t_node *node, int i, char *expanded);
static int	try_word_split(t_node *node, int *i, char *expanded);

void	expand_args(t_node *node, t_env *env_list)
{
	int		i;
	int		has_wildcard;
	char	*expanded;

	i = 0;
	while (node->args[i])
	{
		expanded = expand_string(node->args[i],
				env_list, &has_wildcard);
		if (handle_expand_arg(node, &i, expanded, has_wildcard))
			continue ;
		free(node->args[i]);
		node->args[i] = expanded;
		i++;
	}
}

static int	handle_expand_arg(t_node *node, int *i, char *expanded,
		int has_wildcard)
{
	if (has_wildcard == 1)
	{
		*i = expand_arg_wildcard(node, *i, expanded);
		(*i)++;
		return (1);
	}
	if (expanded[0] == '\0'
		&& !ft_strchr(node->args[*i], '\'')
		&& !ft_strchr(node->args[*i], '"'))
	{
		if (remove_empty_arg(node, *i, expanded))
			return (1);
	}
	if (try_word_split(node, i, expanded))
		return (1);
	return (0);
}

// wildcard expand -> insert args
static int	expand_arg_wildcard(t_node *node, int i, char *expanded)
{
	char	**matches;
	int		count;

	matches = expand_wildcard(expanded);
	count = 0;
	while (matches && matches[count])
		count++;
	free(node->args[i]);
	free(expanded);
	node->args = insert_matches(node->args, i, matches, count);
	if (count > 0)
		return (i + count - 1);
	return (i);
}

static int	remove_empty_arg(t_node *node, int i, char *expanded)
{
	int	j;

	free(node->args[i]);
	free(expanded);
	j = i;
	while (node->args[j + 1])
	{
		node->args[j] = node->args[j + 1];
		j++;
	}
	node->args[j] = NULL;
	return (1);
}

// case: VAR="a b c" and echo $VAR -> echo a b c
static int	try_word_split(t_node *node, int *i, char *expanded)
{
	char	**splits;
	int		count;

	if (!has_unquoted_dollar(node->args[*i])
		|| !ft_strchr(expanded, ' '))
		return (0);
	splits = ft_split_c(expanded, ' ');
	count = 0;
	while (splits && splits[count])
		count++;
	if (count <= 1)
	{
		free_str_array(splits);
		return (0);
	}
	free(node->args[*i]);
	free(expanded);
	node->args = insert_matches(node->args, *i, splits, count);
	*i += count;
	return (1);
}
