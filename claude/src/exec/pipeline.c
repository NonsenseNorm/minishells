/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claude <claude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/03/26 00:00:00 by claude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../core/ms.h"

static int	exit_status(int status)
{
	if (!WIFSIGNALED(status))
		return (WEXITSTATUS(status));
	if (WTERMSIG(status) == SIGINT)
		write(1, "\n", 1);
	if (WTERMSIG(status) == SIGQUIT)
		write(1, "Quit: 3\n", 8);
	return (128 + WTERMSIG(status));
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
			fork_child(sh, &pl->cmds[i], prev, p);
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
	ret = wait_all(pids, pl->count);
	free(pids);
	sig_set_interactive();
	return (ret);
}
