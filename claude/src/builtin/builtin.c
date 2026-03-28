/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claude <claude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/01/01 00:00:00 by claude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin.h"
#include "builtin_internal.h"

int	is_parent_builtin(t_cmd *cmd)
{
	if (!cmd->argv || !cmd->argv[0])
		return (0);
	return (!ft_strcmp(cmd->argv[0], "cd")
		|| !ft_strcmp(cmd->argv[0], "export")
		|| !ft_strcmp(cmd->argv[0], "unset")
		|| !ft_strcmp(cmd->argv[0], "exit"));
}

static int	is_builtin(char *s)
{
	return (!ft_strcmp(s, "echo") || !ft_strcmp(s, "cd")
		|| !ft_strcmp(s, "pwd") || !ft_strcmp(s, "export")
		|| !ft_strcmp(s, "unset") || !ft_strcmp(s, "env")
		|| !ft_strcmp(s, "exit"));
}

int	run_builtin(t_shell *sh, t_cmd *cmd)
{
	if (!cmd->argv || !cmd->argv[0])
		return (0);
	if (!is_builtin(cmd->argv[0]))
		return (-1);
	if (!ft_strcmp(cmd->argv[0], "echo"))
		return (bi_echo(cmd));
	if (!ft_strcmp(cmd->argv[0], "cd"))
		return (bi_cd(sh, cmd));
	if (!ft_strcmp(cmd->argv[0], "pwd"))
		return (bi_pwd());
	if (!ft_strcmp(cmd->argv[0], "export"))
		return (bi_export(sh, cmd));
	if (!ft_strcmp(cmd->argv[0], "unset"))
		return (bi_unset(sh, cmd));
	if (!ft_strcmp(cmd->argv[0], "env"))
		return (bi_env(sh, cmd));
	return (bi_exit(sh, cmd));
}
