/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/03/28 20:28:18 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../core/ms.h"

static int	update_pwd(t_shell *sh, char *oldpwd)
{
	char	cwd[PATH_MAX];

	if (!getcwd(cwd, sizeof(cwd)))
		return (ms_perror("cd", NULL, 1));
	env_set(&sh->env, "OLDPWD", oldpwd, true);
	env_set(&sh->env, "PWD", cwd, true);
	return (0);
}

int	bi_cd(t_shell *sh, t_cmd *cmd)
{
	char	cwd[PATH_MAX];
	char	*target;

	if (cmd->argv[1] != 0x00 && cmd->argv[2])
		return (fprintf(stderr,
				"minishell: cd: too many arguments\n"), 1);
	target = cmd->argv[1];
	if (!target)
		target = env_get(&sh->env, "HOME");
	if (!target)
		return (fprintf(stderr,
				"minishell: cd: HOME not set\n"), 1);
	if (!getcwd(cwd, sizeof(cwd)))
		cwd[0] = 0;
	if (chdir(target) != 0)
		return (ms_perror("cd", target, 1));
	return (update_pwd(sh, cwd));
}
