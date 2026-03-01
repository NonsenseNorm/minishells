/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/02/24 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../core/minishell.h"

static int	open_out(char *path, int app)
{
	if (app)
		return (open(path, O_WRONLY | O_CREAT | O_APPEND, 0644));
	return (open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644));
}

static int	open_redirect(t_shell *sh, t_redirect *r)
{
	if (r->type == REDIRECT_IN)
		return (open(r->target, O_RDONLY));
	if (r->type == REDIRECT_OUT)
		return (open_out(r->target, 0));
	if (r->type == REDIRECT_APPEND)
		return (open_out(r->target, 1));
	return (heredoc_fd(sh, r->target, r->quoted));
}

int	apply_redirects(t_shell *sh, t_redirect *r)
{
	int	fd;

	while (r)
	{
		fd = open_redirect(sh, r);
		if (fd < 0)
		{
			if (r->type == REDIRECT_HEREDOC && sh->exit_code == 130)
				return (130);
			return (ms_perror(r->target, NULL, 1));
		}
		if (r->type == REDIRECT_IN || r->type == REDIRECT_HEREDOC)
			dup2(fd, STDIN_FILENO);
		else
			dup2(fd, STDOUT_FILENO);
		close(fd);
		r = r->next;
	}
	return (0);
}
