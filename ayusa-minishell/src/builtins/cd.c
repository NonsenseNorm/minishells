/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:09:37 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/18 18:38:23 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static int	cd_minus(t_env **env_list, char **path);
static int	resolve_cd_path(char **args, t_env **env_list, char **path);

int	builtin_cd(char **args, t_env **env_list)
{
	char	cwd[PATH_MAX];
	char	*path;

	if (resolve_cd_path(args, env_list, &path))
		return (1);
	getcwd(cwd, sizeof(cwd));
	if (chdir(path) != 0)
	{
		ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
		ft_putstr_fd(path, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putendl_fd(strerror(errno), STDERR_FILENO);
		return (1);
	}
	set_env_value(env_list, "OLDPWD", cwd);
	getcwd(cwd, sizeof(cwd));
	set_env_value(env_list, "PWD", cwd);
	return (0);
}

static int	resolve_cd_path(char **args, t_env **env_list, char **path)
{
	*path = args[1];
	if (args[1] && args[2])
	{
		ft_putendl_fd("minishell: cd: too many arguments", STDERR_FILENO);
		return (1);
	}
	if (!*path)
	{
		*path = get_env_value(*env_list, "HOME");
		if (!*path)
		{
			ft_putendl_fd("minishell: cd: HOME not set", STDERR_FILENO);
			return (1);
		}
	}
	else if (ft_strcmp(*path, "-") == 0)
	{
		if (cd_minus(env_list, path))
			return (1);
	}
	return (0);
}

static int	cd_minus(t_env **env_list, char **path)
{
	*path = get_env_value(*env_list, "OLDPWD");
	if (!(*path))
	{
		ft_putendl_fd("cd: OLDPWD not set", STDERR_FILENO);
		return (1);
	}
	ft_putendl_fd(*path, STDOUT_FILENO);
	return (0);
}
