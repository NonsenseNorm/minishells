/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:28:32 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/18 20:14:23 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static t_env	*parse_env_entry(char *entry);

// **envp -> t_env linked list
t_env	*env_init(char **envp)
{
	t_env	*env_list;
	int		i;

	env_list = NULL;
	if (!envp)
		return (NULL);
	i = 0;
	while (envp[i] != NULL)
	{
		env_add_back(&env_list, parse_env_entry(envp[i]));
		i++;
	}
	env_add_back(&env_list,
		env_new(ft_strdup("?"), ft_strdup("0")));
	return (env_list);
}

static t_env	*parse_env_entry(char *entry)
{
	char	*equal_pos;
	char	*key;
	char	*value;

	equal_pos = ft_strchr(entry, '=');
	if (equal_pos != NULL)
	{
		key = ft_substr(entry, 0, equal_pos - entry);
		value = ft_strdup(equal_pos + 1);
	}
	else
	{
		key = ft_strdup(entry);
		value = NULL;
	}
	return (env_new(key, value));
}

t_env	*env_new(char *key, char *value)
{
	t_env	*new_node;

	new_node = (t_env *)malloc(sizeof(t_env));
	if (!new_node)
	{
		free(key);
		free(value);
		return (NULL);
	}
	new_node->key = key;
	new_node->value = value;
	new_node->next = NULL;
	return (new_node);
}

void	env_add_back(t_env **env_list, t_env *new_node)
{
	t_env	*current;

	if (!env_list || !new_node)
		return ;
	if (*env_list == NULL)
	{
		*env_list = new_node;
		return ;
	}
	current = *env_list;
	while (current->next != NULL)
		current = current->next;
	current->next = new_node;
}

void	free_env(t_env *env_list)
{
	t_env	*current;
	t_env	*next_node;

	current = env_list;
	while (current != NULL)
	{
		next_node = current->next;
		if (current->key != NULL)
			free(current->key);
		if (current->value != NULL)
			free(current->value);
		free(current);
		current = next_node;
	}
}
