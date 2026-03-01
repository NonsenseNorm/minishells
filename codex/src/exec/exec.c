#include "../core/ms.h"

void	child_exec(t_shell *sh, t_cmd *cmd)
{
	char	*path;
	int		ret;

	sig_set_exec_child();
	ret = apply_redirects(sh, cmd->redirects);
	if (ret != 0)
		exit(ret);
	ret = run_builtin(sh, cmd);
	if (ret >= 0)
		exit(ret);
	if (!cmd->argv || !cmd->argv[0])
		exit(0);
	path = find_exec_path(sh, cmd->argv[0]);
	if (!path)
	{
		fprintf(stderr, "minishell: %s: command not found\n", cmd->argv[0]);
		exit(127);
	}
	execve(path, cmd->argv, sh->env.arr);
	if (errno == ENOENT && ft_strchr(cmd->argv[0], '/'))
		exit(ms_perror(cmd->argv[0], NULL, 127));
	if (errno == ENOENT)
		exit(127);
	exit(ms_perror(cmd->argv[0], NULL, 126));
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
	if (pl->count == 1 && is_parent_builtin(&pl->cmds[0]))
		return (exec_single_parent(sh, &pl->cmds[0]));
	if (pl->count == 1 && (!pl->cmds[0].argv || !pl->cmds[0].argv[0]))
		return (apply_redirects(sh, pl->cmds[0].redirects));
	return (exec_forked_pipeline(sh, pl));
}
