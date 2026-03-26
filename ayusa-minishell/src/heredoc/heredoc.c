/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:11:10 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/23 14:15:52 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static int	read_heredoc(t_redirect *redir, t_env *env_list,
				t_node *root);
static void	heredoc_child(t_redirect *redir, t_env *env_list,
				char *tmp_filename, t_node *root);
static void	heredoc_read_loop(t_redirect *redir, t_env *env_list, int fd);
static int	heredoc_parent(t_redirect *redir, char *tmp_filename,
				int status);

// Until it is written to a temporary file
int	process_heredoc(t_node *node, t_env *env_list)
{
	t_redirect	*redir;

	if (node == NULL)
		return (0);
	if (node->type == NODE_CMD)
	{
		redir = node->redirects;
		while (redir != NULL)
		{
			if (redir->type == TK_HEREDOC)
			{
				if (read_heredoc(redir, env_list, node) != 0)
					return (1);
			}
			redir = redir->next;
		}
	}
	else
	{
		if (process_heredoc(node->left, env_list) != 0)
			return (1);
		if (process_heredoc(node->right, env_list) != 0)
			return (1);
	}
	return (0);
}

// fork and read heredoc
static int	read_heredoc(t_redirect *redir, t_env *env_list,
		t_node *root)
{
	pid_t			pid;
	int				status;
	char			*tmp_filename;
	struct termios	term;

	tmp_filename = generate_tmp_filename();
	tcgetattr(STDIN_FILENO, &term);
	signal(SIGINT, SIG_IGN);
	pid = fork();
	if (pid < 0)
		return (1);
	if (pid == 0)
		heredoc_child(redir, env_list, tmp_filename, root);
	waitpid(pid, &status, 0);
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
	return (heredoc_parent(redir, tmp_filename, status));
}

// heredoc child: read lines and write to tmp file
static void	heredoc_child(t_redirect *redir, t_env *env_list,
		char *tmp_filename, t_node *root)
{
	int	fd;

	fd = open(tmp_filename, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	if (fd < 0)
	{
		free(tmp_filename);
		cleanup_and_exit(1, root, env_list);
	}
	set_signal_heredoc();
	heredoc_read_loop(redir, env_list, fd);
	close(fd);
	free(tmp_filename);
	cleanup_and_exit(0, root, env_list);
}

static void	heredoc_read_loop(t_redirect *redir, t_env *env_list, int fd)
{
	char	*line;

	while (1)
	{
		line = readline("ウラ> ");
		if (line == NULL
			|| ft_strcmp(line, redir->filename) == 0)
		{
			free(line);
			break ;
		}
		if (!redir->quoted)
			line = expand_heredoc_line(line, env_list);
		ft_putendl_fd(line, fd);
		free(line);
	}
}

// heredoc parent: handle SIGINT from child
static int	heredoc_parent(t_redirect *redir, char *tmp_filename,
		int status)
{
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
	{
		g_sig = SIGINT;
		write(STDOUT_FILENO, "\n", 1);
		unlink(tmp_filename);
		free(tmp_filename);
		return (1);
	}
	free(redir->filename);
	redir->filename = tmp_filename;
	return (0);
}
