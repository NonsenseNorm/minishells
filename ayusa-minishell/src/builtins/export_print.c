/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_print.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 18:31:02 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/18 20:13:04 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static void		print_sorted_keys(t_env *env_list, char **keys, int count);
static void		print_single_export(t_env *node);
static void		print_escaped_value(char *value);

void	print_exported_env(t_env *env_list)
{
	int		count;
	int		i;
	char	**keys;
	t_env	*node;

	count = count_env_nodes(env_list);
	keys = (char **)malloc(sizeof(char *) * (count + 1));
	if (!keys)
		return ;
	i = 0;
	node = env_list;
	while (node)
	{
		keys[i++] = node->key;
		node = node->next;
	}
	sort_str_array(keys, count);
	print_sorted_keys(env_list, keys, count);
	free(keys);
}

static void	print_sorted_keys(t_env *env_list, char **keys, int count)
{
	int		i;
	t_env	*node;

	i = 0;
	while (i < count)
	{
		node = env_list;
		while (node && ft_strcmp(node->key, keys[i]) != 0)
			node = node->next;
		if (node && ft_strcmp(node->key, "?") != 0)
			print_single_export(node);
		i++;
	}
}

static void	print_single_export(t_env *node)
{
	ft_putstr_fd("declare -x ", STDOUT_FILENO);
	ft_putstr_fd(node->key, STDOUT_FILENO);
	if (node->value)
	{
		ft_putstr_fd("=\"", STDOUT_FILENO);
		print_escaped_value(node->value);
		ft_putchar_fd('\"', STDOUT_FILENO);
	}
	ft_putchar_fd('\n', STDOUT_FILENO);
}

static void	print_escaped_value(char *value)
{
	while (*value)
	{
		if (*value == '"' || *value == '\\'
			|| *value == '$' || *value == '`')
			ft_putchar_fd('\\', STDOUT_FILENO);
		ft_putchar_fd(*value, STDOUT_FILENO);
		value++;
	}
}
