/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_getset.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claude <claude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/01/01 00:00:00 by claude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../core/ms.h"

static int	find_key(t_env *env, const char *key)
{
	int	klen;
	int	i;

	klen = ft_strlen(key);
	i = 0;
	while (i < env->len)
	{
		if (!ft_strncmp(env->arr[i], key, klen) && env->arr[i][klen] == '=')
			return (i);
		i++;
	}
	return (-1);
}

char	*env_get(t_env *env, const char *key)
{
	int	idx;
	int	klen;

	idx = find_key(env, key);
	if (idx < 0)
		return (NULL);
	klen = ft_strlen(key);
	return (env->arr[idx] + klen + 1);
}

static char	*make_kv(const char *key, const char *value)
{
	char	*kv;
	size_t	klen;
	size_t	vlen;

	klen = ft_strlen(key);
	vlen = ft_strlen(value);
	kv = malloc(klen + vlen + 2);
	if (!kv)
		return (NULL);
	ft_memcpy(kv, key, klen);
	kv[klen] = '=';
	ft_memcpy(kv + klen + 1, value, vlen);
	kv[klen + vlen + 1] = 0;
	return (kv);
}

int	env_set(t_env *env, const char *key, const char *value, bool export)
{
	char	*kv;
	int		idx;

	(void)export;
	kv = make_kv(key, value);
	if (!kv)
		return (1);
	idx = find_key(env, key);
	if (idx >= 0)
	{
		free(env->arr[idx]);
		env->arr[idx] = kv;
		return (0);
	}
	if (env->len + 2 >= env->cap && env_grow(env) != 0)
		return (free(kv), 1);
	env->arr[env->len++] = kv;
	env->arr[env->len] = NULL;
	return (0);
}

int	env_unset(t_env *env, const char *key)
{
	int	idx;

	idx = find_key(env, key);
	if (idx < 0)
		return (0);
	free(env->arr[idx]);
	while (idx + 1 < env->len)
	{
		env->arr[idx] = env->arr[idx + 1];
		idx++;
	}
	env->len--;
	env->arr[env->len] = NULL;
	return (0);
}
