/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claude <claude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/01/01 00:00:00 by claude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../core/ms.h"

static int	wait_signal_exit(int last)
{
	if (WTERMSIG(last) == SIGINT)
		write(1, "\n", 1);
	if (WTERMSIG(last) == SIGQUIT)
		write(1, "Quit: 3\n", 8);
	return (128 + WTERMSIG(last));
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
	if (WIFSIGNALED(last))
		return (wait_signal_exit(last));
	return (WEXITSTATUS(last));
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
	child_exec(sh, cmd);
}

static int	do_pipes(t_shell *sh, t_pipeline *pl, pid_t *pids, int *out_prev)
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
			fork_child(sh, &pl->cmds[i], prev, p);
		if (prev != -1)
			close(prev);
		if (p[1] != -1)
			close(p[1]);
		prev = p[0];
		i++;
	}
	*out_prev = prev;
	return (0);
}

int	exec_forked_pipeline(t_shell *sh, t_pipeline *pl)
{
	pid_t	*pids;
	int		prev;
	int		i;

	pids = ms_alloc(&sh->mem, sizeof(pid_t) * pl->count);
	sig_set_exec_parent();
	if (do_pipes(sh, pl, pids, &prev) != 0)
		return (1);
	if (prev != -1)
		close(prev);
	i = wait_all(pids, pl->count);
	sig_set_interactive();
	return (i);
}
