#include "types.h"

extern char	**environ;

/* ================================================================
   実装済みユーティリティ (変更不要)
   ================================================================ */

/*
** free_arr -- NULL 終端の文字列配列をすべて解放する。
*/
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

/*
** split_path -- PATH 環境変数文字列を ':' で分割して NULL 終端配列にする。
**
**   例: "/usr/bin:/bin:/usr/local/bin"
**       → { "/usr/bin", "/bin", "/usr/local/bin", NULL }
*/
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
   TODO: 以下の関数を実装してください。
   実装の手順は EXEC_GUIDE.md を参照してください。
   ================================================================ */

/*
** find_cmd -- コマンド名からフルパスを返す。
**
** argv0 にスラッシュが含まれる場合: strdup(argv0) をそのまま返す。
** 含まれない場合: $PATH の各ディレクトリを順に検索し、
**   access(candidate, X_OK) == 0 の最初のパスを返す。
** 見つからない場合は NULL を返す。
**
** Step 1 で実装する。
*/
char	*find_cmd(const char *argv0)
{
	char	**dirs;
	char	*path_env;
	char	*candidate;
	int		i;
	size_t	len;

	if (strchr(argv0, '/'))
		return (NULL); /* TODO: Step 1 — strdup(argv0) を返す */
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
		/* TODO: Step 1
		**   candidate = malloc(len) して snprintf で "dirs[i]/argv0" を作成し
		**   access(candidate, X_OK) == 0 でなければ free して NULL にする
		*/
		candidate = malloc(len);
        snprintf(candidate, len, "%s/%s", dirs[i], argv0);
		if (access(candidate, X_OK) != 0)
		{
			free(candidate);
			candidate = 0x00;
		}
		i++;
	}
	free_arr(dirs);
	return (candidate);
}

/*
** setup_child_io -- 子プロセスで stdin/stdout をパイプに差し替える。
**
**   pipe_in  >= 0: dup2(pipe_in,  STDIN_FILENO)  の後 close(pipe_in)
**   pipe_out >= 0: dup2(pipe_out, STDOUT_FILENO) の後 close(pipe_out)
**
** Step 2 で実装する。
*/
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

/*
** apply_redirections -- リダイレクトリストに従ってファイルを開き fd を繋ぐ。
**
**   REDIRECT_IN:     open(O_RDONLY)              → dup2 to STDIN_FILENO
**   REDIRECT_OUT:    open(O_WRONLY|O_CREAT|O_TRUNC, 0644) → dup2 to STDOUT_FILENO
**   REDIRECT_APPEND: open(O_WRONLY|O_CREAT|O_APPEND, 0644) → dup2 to STDOUT_FILENO
**
** open が失敗したら perror(r->target) して 1 を返す。
**
** Step 3 で実装する。
*/
static int	apply_redirections(t_redirect *r)
{
	int	fd;

	while (r)
	{
		if (r->type == REDIRECT_IN)
			fd = open(r->target, O_RDONLY);
		else if (r->type == REDIRECT_OUT)
			fd = open(r->target, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		else if (r->type == REDIRECT_APPEND)
			fd = open(r->target, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else
	}
}

/*
** exec_one -- 1コマンドを子プロセスとして起動し pid を返す。
**
** 子プロセス:
**   1. setup_child_io(pipe_in, pipe_out)
**   2. apply_redirections(cmd->redirects) が失敗したら exit(1)
**   3. find_cmd(cmd->argv[0]) でパスを解決
**      見つからなければ "command not found" を stderr に出して exit(127)
**   4. execve(path, cmd->argv, environ) を呼ぶ
**      失敗したら perror(path) して exit(126)
**
** 親プロセス: fork した pid を返す。
**
** Step 4 で実装する。
*/
static pid_t	exec_one(t_cmd *cmd, int pipe_in, int pipe_out)
{
	/* TODO: Step 4 */
	(void)cmd;
	(void)pipe_in;
	(void)pipe_out;
	return (-1);
}

/*
** exec_pipeline -- パイプラインを実行して最後のコマンドの終了コードを返す。
**
** 以下の骨格を完成させること (Step 5 参照):
**   ・コマンドごとに pipe() を呼ぶ (最後のコマンドは不要)
**   ・exec_one を呼んで pids[i] に保存
**   ・親プロセスで prev_fd と pipe_fd[1] を close し prev_fd を更新
**
** Step 5 で実装する。
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
	/* Step 4/5 実装前のコンパイルエラー防止 (実装後に削除してよい) */
	if (0)
	{
		setup_child_io(-1, -1);
		apply_redirections(NULL);
		exec_one(NULL, -1, -1);
	}
	prev_fd = -1;
	i = 0;
	while (i < pl->count)
	{
		/* TODO: Step 5
		**   if (i < pl->count - 1): pipe(pipe_fd) を呼ぶ
		**   else: pipe_fd[0] = pipe_fd[1] = -1
		**   pids[i] = exec_one(&pl->cmds[i], prev_fd, pipe_fd[1])
		**   if (prev_fd >= 0) close(prev_fd)
		**   if (pipe_fd[1] >= 0) close(pipe_fd[1])
		**   prev_fd = pipe_fd[0]
		*/
		(void)pipe_fd;
		pids[i] = -1;
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
