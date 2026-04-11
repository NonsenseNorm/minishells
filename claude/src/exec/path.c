/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/04/11 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec_internal.h"
#include "../env/env.h"

static char	*join_path(const char *dir, const char *cmd)
{
	char	*tmp;
	char	*out;

	tmp = ft_strjoin(dir, "/");
	if (!tmp)
		return (NULL);
	out = ft_strjoin(tmp, cmd);
	free(tmp);
	return (out);
}

static size_t	count_segs(const char *s)
{
	size_t	n;

	n = 1;
	while (*s)
	{
		if (*s == ':')
			n++;
		s++;
	}
	return (n);
}

static char	*get_seg(const char *s, size_t st, size_t len)
{
	if (len == 0)
		return (ft_strdup("."));
	return (ft_substr(s, (unsigned int)st, len));
}

static char	**split_path(const char *s)
{
	char	**arr;
	size_t	k;
	size_t	st;
	size_t	i;

	arr = ft_calloc(count_segs(s) + 1, sizeof(char *));
	if (!arr)
		return (NULL);
	i = 0;
	k = 0;
	st = 0;
	while (1)
	{
		if (s[i] == ':' || !s[i])
		{
			arr[k] = get_seg(s, st, i - st);
			if (!arr[k++])
				return (ft_free_split(arr), NULL);
			st = i + 1;
		}
		if (!s[i])
			break ;
		i++;
	}
	return (arr);
}

char	*find_exec_path(t_shell *sh, char *cmd)
{
	char	**parts;
	char	*path;
	int		i;

	if (ft_strchr(cmd, '/'))
		return (ft_strdup(cmd));
	path = env_get(&sh->env, "PATH");
	if (!path)
		return (NULL);
	parts = split_path(path);
	if (!parts)
		return (NULL);
	i = 0;
	while (parts[i])
	{
		path = join_path(parts[i], cmd);
		if (path && access(path, F_OK) == 0)
			return (ft_free_split(parts), path);
		free(path);
		i++;
	}
	ft_free_split(parts);
	return (NULL);
}
