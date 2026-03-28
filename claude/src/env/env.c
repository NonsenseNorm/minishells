/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/01/01 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env.h"

int	env_grow(t_env *env)
{
	char	**new_arr;
	int		new_cap;

	new_cap = env->cap * 2;
	new_arr = ft_calloc(new_cap, sizeof(char *));
	if (!new_arr)
		return (1);
	ft_memcpy(new_arr, env->arr, sizeof(char *) * env->len);
	free(env->arr);
	env->arr = new_arr;
	env->cap = new_cap;
	return (0);
}

int	env_init(t_env *env, char **environ)
{
	int	i;

	env->len = 0;
	env->cap = 64;
	env->arr = ft_calloc(env->cap, sizeof(char *));
	if (!env->arr)
		return (1);
	i = 0;
	while (environ && environ[i])
	{
		if (env->len + 2 >= env->cap && env_grow(env) != 0)
			return (1);
		env->arr[env->len++] = ft_strdup(environ[i]);
		i++;
	}
	env->arr[env->len] = NULL;
	return (0);
}

void	env_free(t_env *env)
{
	int	i;

	i = 0;
	while (i < env->len)
		free(env->arr[i++]);
	free(env->arr);
}
