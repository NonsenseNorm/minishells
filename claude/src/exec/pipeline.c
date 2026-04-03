/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/03/26 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec_internal.h"
#include "../signal/signal.h"
#include "../heredoc/heredoc.h"

static int	exit_status(int status)
{
	int	sig;

	if (WIFSIGNALED(status))
	{
		sig = WTERMSIG(status);
		if (sig == SIGINT)
			write(STDERR_FILENO, "\n", 1);
		else if (sig == SIGQUIT)
		{
			write(STDERR_FILENO, "Quit", 4);
			if (WCOREDUMP(status))
				write(STDERR_FILENO, " (core dumped)", 14);
			write(STDERR_FILENO, "\n", 1);
		}
		return (128 + sig);
	}
	return (WEXITSTATUS(status));
}

static int	wait_all(pid_t *pids, int n)
{
	int	status;
	int	last;
	int	i;
	int	r;

	last = 0;
	i = 0;
	while (i < n)
	{
		r = waitpid(pids[i], &status, 0);
		if (r < 0 && errno == EINTR)
			continue ;
		if (r < 0)
			return (1);
		if (i == n - 1)
			last = status;
		i++;
	}
	return (exit_status(last));
}

static void	fork_child(t_shell *sh, t_cmd *cmd, int prev, int p[2])
{
	if (prev != -1)
	{
		dup2(prev, STDIN_FILENO);
		close(prev);
	}
	if (p[1] != -1)
	{
		dup2(p[1], STDOUT_FILENO);
		close(p[0]);
		close(p[1]);
	}
	sh->interactive = false;
	child_exec(sh, cmd);
}

static int	fork_all(t_shell *sh, t_pipeline *pl, pid_t *pids)
{
	int	p[2];
	int	prev;
	int	i;

	prev = -1;
	i = 0;
	while (i < pl->count)
	{
		p[0] = -1;
		p[1] = -1;
		if (i < pl->count - 1 && pipe(p) != 0)
			return (1);
		pids[i] = fork();
		if (pids[i] == 0)
			(free(pids), fork_child(sh, &pl->cmds[i], prev, p));
		if (prev != -1)
			close(prev);
		if (p[1] != -1)
			close(p[1]);
		prev = p[0];
		i++;
	}
	if (prev != -1)
		close(prev);
	return (0);
}

int	exec_forked_pipeline(t_shell *sh, t_pipeline *pl)
{
	pid_t	*pids;
	int		ret;

	pids = malloc(sizeof(pid_t) * pl->count);
	if (!pids)
		return (1);
	sig_set_exec_parent();
	if (fork_all(sh, pl, pids) != 0)
		return (free(pids), 1);
	close_pipeline_heredocs(pl);
	ret = wait_all(pids, pl->count);
	free(pids);
	if (sh->interactive)
		sig_set_interactive();
	else
		sig_set_noninteractive();
	return (ret);
}
