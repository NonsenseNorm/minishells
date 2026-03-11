#include "types.h"

extern char	**environ;

/* ================================================================
   実装済みユーティリティ (変更不要)
   ================================================================ */

static void	free_arr(char **arr)
{
	int	i;

	if (!arr)
		return ;
	i = 0;
	while (arr[i])
		free(arr[i++]);
	free(arr);
}

static char	**split_path(const char *path_env)
{
	int			count;
	int			i;
	char		**arr;
	const char	*s;
	const char	*e;

	count = 1;
	s = path_env;
	while (*s)
	{
		if (*s++ == ':')
			count++;
	}
	arr = malloc(sizeof(char *) * (size_t)(count + 1));
	if (!arr)
		return (NULL);
	s = path_env;
	i = 0;
	while (i < count)
	{
		e = s;
		while (*e && *e != ':')
			e++;
		arr[i] = malloc((size_t)(e - s) + 1);
		if (!arr[i])
		{
			arr[i] = NULL;
			free_arr(arr);
			return (NULL);
		}
		memcpy(arr[i], s, (size_t)(e - s));
		arr[i][e - s] = '\0';
		s = (*e == ':') ? e + 1 : e;
		i++;
	}
	arr[count] = NULL;
	return (arr);
}

/* ================================================================
   exec.c の完成形 (practice2 の解答)
   ヒアドック対応を追加済み
   ================================================================ */

char	*find_cmd(const char *argv0)
{
	char	**dirs;
	char	*path_env;
	char	*candidate;
	int		i;
	size_t	len;

	if (strchr(argv0, '/'))
		return (strdup(argv0));
	path_env = getenv("PATH");
	if (!path_env)
		return (NULL);
	dirs = split_path(path_env);
	if (!dirs)
		return (NULL);
	i = 0;
	candidate = NULL;
	while (dirs[i] && !candidate)
	{
		len = strlen(dirs[i]) + 1 + strlen(argv0) + 1;
		candidate = malloc(len);
		if (candidate)
		{
			snprintf(candidate, len, "%s/%s", dirs[i], argv0);
			if (access(candidate, X_OK) != 0)
			{
				free(candidate);
				candidate = NULL;
			}
		}
		i++;
	}
	free_arr(dirs);
	return (candidate);
}

/*
** apply_redirections -- リダイレクトリストを適用する。
**   REDIRECT_HEREDOC: r->hd_fd (heredoc.c がセット済み) を STDIN_FILENO に dup2
*/
static int	apply_redirections(t_redirect *r)
{
	int	fd;

	while (r)
	{
		if (r->type == REDIRECT_IN)
		{
			fd = open(r->target, O_RDONLY);
			if (fd == -1) { perror(r->target); return (1); }
			if (dup2(fd, STDIN_FILENO) == -1) return (1);
			close(fd);
		}
		else if (r->type == REDIRECT_OUT)
		{
			fd = open(r->target, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd == -1) { perror(r->target); return (1); }
			if (dup2(fd, STDOUT_FILENO) == -1) return (1);
			close(fd);
		}
		else if (r->type == REDIRECT_APPEND)
		{
			fd = open(r->target, O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (fd == -1) { perror(r->target); return (1); }
			if (dup2(fd, STDOUT_FILENO) == -1) return (1);
			close(fd);
		}
		else if (r->type == REDIRECT_HEREDOC)
		{
			/* heredoc.c の prepare_pipeline_heredocs がパイプ読み端をセット済み */
			if (r->hd_fd >= 0)
			{
				if (dup2(r->hd_fd, STDIN_FILENO) == -1)
					return (1);
				close(r->hd_fd);
			}
		}
		r = r->next;
	}
	return (0);
}

static void	setup_child_io(int pipe_in, int pipe_out)
{
	if (pipe_in >= 0)
	{
		dup2(pipe_in, STDIN_FILENO);
		close(pipe_in);
	}
	if (pipe_out >= 0)
	{
		dup2(pipe_out, STDOUT_FILENO);
		close(pipe_out);
	}
}

static pid_t	exec_one(t_cmd *cmd, int pipe_in, int pipe_out)
{
	pid_t	pid;
	char	*path;

	pid = fork();
	if (pid < 0) { perror("fork"); return (-1); }
	if (pid == 0)
	{
		setup_child_io(pipe_in, pipe_out);
		if (apply_redirections(cmd->redirects) != 0)
			exit(1);
		if (!cmd->argv || !cmd->argv[0])
			exit(0);
		path = find_cmd(cmd->argv[0]);
		if (!path)
		{
			fprintf(stderr, "minishell: %s: command not found\n",
				cmd->argv[0]);
			exit(127);
		}
		execve(path, cmd->argv, environ);
		perror(path);
		free(path);
		exit(126);
	}
	return (pid);
}

/*
** exec_pipeline -- ヒアドックの準備をしてからパイプラインを実行する。
**
** prepare_pipeline_heredocs は heredoc.c に実装する (Step 3)。
*/
int	exec_pipeline(t_pipeline *pl)
{
	int		i;
	int		prev_fd;
	int		pipe_fd[2];
	pid_t	*pids;
	int		status;
	int		last_status;

	if (pl->count == 0)
		return (0);
	pids = malloc(sizeof(pid_t) * (size_t)pl->count);
	if (!pids)
		return (1);
	/* ヒアドックを先に読み込んでパイプを準備する (Step 3 が必要) */
	if (prepare_pipeline_heredocs(pl) != 0)
	{
		free(pids);
		return (1);
	}
	prev_fd = -1;
	i = 0;
	while (i < pl->count)
	{
		if (i < pl->count - 1)
		{
			if (pipe(pipe_fd) == -1) { perror("pipe"); free(pids); return (1); }
		}
		else
			pipe_fd[0] = pipe_fd[1] = -1;
		pids[i] = exec_one(&pl->cmds[i], prev_fd, pipe_fd[1]);
		if (prev_fd >= 0) close(prev_fd);
		if (pipe_fd[1] >= 0) close(pipe_fd[1]);
		prev_fd = pipe_fd[0];
		i++;
	}
	if (prev_fd >= 0)
		close(prev_fd);
	last_status = 0;
	i = 0;
	while (i < pl->count)
	{
		if (pids[i] > 0)
		{
			waitpid(pids[i], &status, 0);
			last_status = WEXITSTATUS(status);
		}
		i++;
	}
	free(pids);
	return (last_status);
}
