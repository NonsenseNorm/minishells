/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_access.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 18:23:36 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/21 16:33:43 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static int	update_env_value(t_env *node, char *value);

char	*get_env_value(t_env *env_list, char *key)
{
	if (!env_list || !key)
		return (NULL);
	while (env_list)
	{
		if (ft_strcmp(env_list->key, key) == 0)
			return (env_list->value);
		env_list = env_list->next;
	}
	return (NULL);
}

void	set_env_value(t_env **env_list, char *key, char *value)
{
	t_env	*current;
	char	*new_value;

	if (!env_list || !key)
		return ;
	current = *env_list;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
		{
			update_env_value(current, value);
			return ;
		}
		current = current->next;
	}
	if (value)
		new_value = ft_strdup(value);
	else
		new_value = NULL;
	env_add_back(env_list, env_new(ft_strdup(key), new_value));
}

static int	update_env_value(t_env *node, char *value)
{
	char	*new_value;

	new_value = NULL;
	if (value)
	{
		new_value = ft_strdup(value);
		if (!new_value)
			return (1);
	}
	free(node->value);
	node->value = new_value;
	return (0);
}
