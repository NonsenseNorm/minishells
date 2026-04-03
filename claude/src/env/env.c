/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/04/03 22:02:17 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env.h"

int	env_grow(t_env *env)
{
	t_var	*nv;
	int		new_cap;

	new_cap = env->cap * 2;
	nv = ft_calloc(new_cap, sizeof(t_var));
	if (!nv)
		return (1);
	ft_memcpy(nv, env->vars, sizeof(t_var) * env->len);
	free(env->vars);
	env->vars = nv;
	env->cap = new_cap;
	return (0);
}

static int	init_one(t_env *env, char *entry)
{
	char	*eq;
	int		klen;

	if (env->len + 2 >= env->cap && env_grow(env) != 0)
		return (1);
	eq = ft_strchr(entry, '=');
	if (!eq)
		return (0);
	klen = eq - entry;
	env->vars[env->len].key = ft_substr(entry, 0, klen);
	env->vars[env->len].val = ft_strdup(eq + 1);
	env->vars[env->len].exported = true;
	if (!env->vars[env->len].key || !env->vars[env->len].val)
		return (1);
	env->len++;
	return (0);
}

int	env_init(t_env *env, char **environ)
{
	int	i;

	env->len = 0;
	env->cap = 64;
	env->vars = ft_calloc(env->cap, sizeof(t_var));
	if (!env->vars)
		return (1);
	i = 0;
	while (environ && environ[i])
	{
		if (init_one(env, environ[i]) != 0)
			return (1);
		i++;
	}
	return (0);
}

void	env_free(t_env *env)
{
	int	i;

	i = 0;
	while (i < env->len)
	{
		free(env->vars[i].key);
		free(env->vars[i].val);
		i++;
	}
	free(env->vars);
}

char	**env_to_arr(t_env *env)
{
	char	**arr;
	char	*tmp;
	int		i;
	int		j;

	arr = malloc(sizeof(char *) * (env->len + 1));
	if (!arr)
		return (NULL);
	j = 0;
	i = -1;
	while (++i < env->len)
	{
		if (!env->vars[i].exported || !env->vars[i].val)
			continue ;
		tmp = ft_strjoin(env->vars[i].key, "=");
		arr[j] = ft_strjoin(tmp, env->vars[i].val);
		free(tmp);
		if (!arr[j])
			return (NULL);
		j++;
	}
	arr[j] = NULL;
	return (arr);
}
