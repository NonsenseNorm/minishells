/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:11:35 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/18 20:17:07 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/minishell.h"

static int	handle_redir_node(t_redirect *current);
static int	handle_input(t_redirect *redir);
static int	handle_output(t_redirect *redir);

int	apply_redirects(t_redirect *redirects)
{
	t_redirect	*current;

	current = redirects;
	while (current)
	{
		if (handle_redir_node(current) != 0)
			return (1);
		current = current->next;
	}
	return (0);
}

static int	handle_redir_node(t_redirect *current)
{
	if (current->type != TK_HEREDOC
		&& (!current->filename || !current->filename[0]))
		return (1);
	if (current->type == TK_REDIR_IN
		|| current->type == TK_HEREDOC)
	{
		if (handle_input(current) < 0)
			return (1);
	}
	else if (current->type == TK_REDIR_OUT
		|| current->type == TK_REDIR_APPEND)
	{
		if (handle_output(current) < 0)
			return (1);
	}
	return (0);
}

static int	handle_input(t_redirect *redir)
{
	int	fd;

	fd = open(redir->filename, O_RDONLY);
	if (fd < 0)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		perror(redir->filename);
		return (-1);
	}
	if (redir->type == TK_HEREDOC)
		unlink(redir->filename);
	dup2(fd, STDIN_FILENO);
	close(fd);
	return (0);
}

static int	handle_output(t_redirect *redir)
{
	int	fd;

	if (redir->type == TK_REDIR_OUT)
		fd = open(redir->filename,
				O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else
		fd = open(redir->filename,
				O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd < 0)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		perror(redir->filename);
		return (-1);
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
	return (0);
}

void	restore_fds(int saved_stdin, int saved_stdout)
{
	dup2(saved_stdin, STDIN_FILENO);
	close(saved_stdin);
	dup2(saved_stdout, STDOUT_FILENO);
	close(saved_stdout);
}
