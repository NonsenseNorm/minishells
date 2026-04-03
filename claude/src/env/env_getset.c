/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_getset.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/01/01 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env.h"

static int	find_key(t_env *env, const char *key)
{
	int	i;

	i = 0;
	while (i < env->len)
	{
		if (!ft_strcmp(env->vars[i].key, key))
			return (i);
		i++;
	}
	return (-1);
}

char	*env_get(t_env *env, const char *key)
{
	int	idx;

	idx = find_key(env, key);
	if (idx < 0)
		return (NULL);
	return (env->vars[idx].val);
}

static int	set_new(t_env *env, const char *key, const char *val, bool exp)
{
	if (env->len + 2 >= env->cap && env_grow(env) != 0)
		return (1);
	env->vars[env->len].key = ft_strdup(key);
	if (!env->vars[env->len].key)
		return (1);
	env->vars[env->len].val = NULL;
	if (val)
	{
		env->vars[env->len].val = ft_strdup(val);
		if (!env->vars[env->len].val)
			return (free(env->vars[env->len].key), 1);
	}
	env->vars[env->len].exported = exp;
	env->len++;
	return (0);
}

int	env_set(t_env *env, const char *key, const char *val, bool exp)
{
	int	idx;

	idx = find_key(env, key);
	if (idx < 0)
		return (set_new(env, key, val, exp));
	if (val)
	{
		free(env->vars[idx].val);
		env->vars[idx].val = ft_strdup(val);
		if (!env->vars[idx].val)
			return (1);
	}
	if (exp)
		env->vars[idx].exported = true;
	return (0);
}

int	env_unset(t_env *env, const char *key)
{
	int	idx;

	idx = find_key(env, key);
	if (idx < 0)
		return (0);
	free(env->vars[idx].key);
	free(env->vars[idx].val);
	while (idx + 1 < env->len)
	{
		env->vars[idx] = env->vars[idx + 1];
		idx++;
	}
	env->len--;
	ft_memset(&env->vars[env->len], 0, sizeof(t_var));
	return (0);
}
