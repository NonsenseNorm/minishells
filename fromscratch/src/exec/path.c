/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/02/24 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../core/minishell.h"

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

char	*find_exec_path(t_shell *sh, char *cmd)
{
	char	**parts;
	char	*path;
	int		i;

	if (!*cmd)
		return (NULL);
	if (ft_strchr(cmd, '/'))
		return (ft_strdup(cmd));
	path = env_get(&sh->env, "PATH");
	if (!path)
		return (NULL);
	parts = ft_split(path, ':');
	if (!parts)
		return (NULL);
	i = 0;
	while (parts[i])
	{
		path = join_path(parts[i], cmd);
		if (path && access(path, X_OK) == 0)
			return (ft_free_split(parts), path);
		free(path);
		i++;
	}
	ft_free_split(parts);
	return (NULL);
}
