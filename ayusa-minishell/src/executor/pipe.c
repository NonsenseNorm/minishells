/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:11:33 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/18 20:16:50 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static void	exec_pipe_left(t_node *n, t_env **el, int fd[2], t_node *rt);
static void	exec_pipe_right(t_node *n, t_env **el, int fd[2], t_node *rt);

int	exec_pipeline(t_node *node, t_env **env_list, t_node *root)
{
	int		fd[2];
	pid_t	pids[2];
	int		status;

	if (pipe(fd) < 0)
	{
		perror("minishell: pipe");
		return (1);
	}
	pids[0] = fork();
	if (pids[0] == 0)
		exec_pipe_left(node, env_list, fd, root);
	pids[1] = fork();
	if (pids[1] == 0)
		exec_pipe_right(node, env_list, fd, root);
	close(fd[0]);
	close(fd[1]);
	waitpid(pids[0], NULL, 0);
	waitpid(pids[1], &status, 0);
	return (calculate_exit_status_quit(status));
}

static void	exec_pipe_left(t_node *n, t_env **el, int fd[2], t_node *rt)
{
	int	status;

	set_signal_child();
	close(fd[0]);
	dup2(fd[1], STDOUT_FILENO);
	close(fd[1]);
	status = exec_ast(n->left, el, rt);
	cleanup_and_exit(status, rt, *el);
}

static void	exec_pipe_right(t_node *n, t_env **el, int fd[2], t_node *rt)
{
	int	status;

	set_signal_child();
	close(fd[1]);
	dup2(fd[0], STDIN_FILENO);
	close(fd[0]);
	status = exec_ast(n->right, el, rt);
	cleanup_and_exit(status, rt, *el);
}
