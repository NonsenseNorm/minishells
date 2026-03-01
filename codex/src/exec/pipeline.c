#include "../core/ms.h"

static void	setup_pipe(int prev, int p[2], int i, int n)
{
	if (prev != -1)
		dup2(prev, STDIN_FILENO);
	if (i < n - 1)
		dup2(p[1], STDOUT_FILENO);
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
	{
		if (WTERMSIG(last) == SIGINT)
			write(1, "\n", 1);
		if (WTERMSIG(last) == SIGQUIT)
			write(1, "Quit: 3\n", 8);
		return (128 + WTERMSIG(last));
	}
	return (WEXITSTATUS(last));
}

int	exec_forked_pipeline(t_shell *sh, t_pipeline *pl)
{
	pid_t	*pids;
	int		p[2];
	int		prev;
	int		i;

	pids = ms_alloc(&sh->mem, sizeof(pid_t) * pl->count);
	prev = -1;
	i = 0;
	sig_set_exec_parent();
	while (i < pl->count)
	{
		if (i < pl->count - 1 && pipe(p) != 0)
			return (1);
		pids[i] = fork();
		if (pids[i] == 0)
		{
			setup_pipe(prev, p, i, pl->count);
			if (prev != -1)
				close(prev);
			if (i < pl->count - 1)
				(close(p[0]), close(p[1]));
			child_exec(sh, &pl->cmds[i]);
		}
		if (prev != -1)
			close(prev);
		if (i < pl->count - 1)
			(close(p[1]), prev = p[0]);
		i++;
	}
	if (prev != -1)
		close(prev);
	i = wait_all(pids, pl->count);
	sig_set_interactive();
	return (i);
}
