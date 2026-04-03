/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/01/01 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec_internal.h"
#include "../signal/signal.h"
#include "../heredoc/heredoc.h"
#include "../builtin/builtin.h"
#include "../core/core.h"
#include "../env/env.h"
#include "../mem/mem.h"

static void	child_exit(t_shell *sh, int code)
{
	if (sh->cur_mem)
		mem_reset(sh->cur_mem);
	free(sh->cur_input);
	env_free(&sh->env);
	exit(code);
}

static void	exec_not_found(t_shell *sh, t_cmd *cmd)
{
	char	*path;
	char	**envp;

	path = find_exec_path(sh, cmd->argv[0]);
	if (!path)
	{
		ms_err("minishell: ");
		ms_err(cmd->argv[0]);
		ms_err(": command not found\n");
		child_exit(sh, 127);
	}
	envp = env_to_arr(&sh->env);
	if (sh->cur_mem)
		mem_reset(sh->cur_mem);
	free(sh->cur_input);
	env_free(&sh->env);
	execve(path, cmd->argv, envp);
	ft_free_split(envp);
	free(path);
	if (errno == ENOENT && ft_strchr(cmd->argv[0], '/'))
		exit(ms_perror(cmd->argv[0], NULL, 127));
	if (errno == ENOENT)
		exit(127);
	exit(ms_perror(cmd->argv[0], NULL, 126));
}

void	child_exec(t_shell *sh, t_cmd *cmd)
{
	int	ret;

	sig_set_exec_child();
	ret = apply_redirects(sh, cmd->redirects);
	if (ret != 0)
		child_exit(sh, ret);
	ret = run_builtin(sh, cmd);
	if (ret >= 0)
		child_exit(sh, ret);
	if (!cmd->argv || !cmd->argv[0])
		child_exit(sh, 0);
	if (!cmd->argv[0][0])
	{
		ms_err("minishell: : command not found\n");
		child_exit(sh, 127);
	}
	exec_not_found(sh, cmd);
}

static int	exec_single_parent(t_shell *sh, t_cmd *cmd)
{
	int	in_save;
	int	out_save;
	int	ret;

	in_save = dup(STDIN_FILENO);
	out_save = dup(STDOUT_FILENO);
	ret = apply_redirects(sh, cmd->redirects);
	if (ret == 0)
		ret = run_builtin(sh, cmd);
	dup2(in_save, STDIN_FILENO);
	dup2(out_save, STDOUT_FILENO);
	close(in_save);
	close(out_save);
	if (ret < 0)
		ret = 1;
	return (ret);
}

int	exec_pipeline(t_shell *sh, t_pipeline *pl)
{
	int	ret;

	sig_set_exec_parent();
	if (gather_heredocs(sh, pl) != 0)
		return (sig_set_interactive(), sh->exit_code);
	if (pl->count == 1 && is_parent_builtin(&pl->cmds[0]))
		ret = exec_single_parent(sh, &pl->cmds[0]);
	else if (pl->count == 1 && (!pl->cmds[0].argv || !pl->cmds[0].argv[0]))
		ret = apply_redirects(sh, pl->cmds[0].redirects);
	else
		ret = exec_forked_pipeline(sh, pl);
	if (g_sig == SIGINT)
	{
		write(STDERR_FILENO, "\n", 1);
		ret = 130;
	}
	sig_set_interactive();
	return (ret);
}
