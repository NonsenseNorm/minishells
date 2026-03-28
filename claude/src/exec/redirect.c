/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claude <claude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/01/01 00:00:00 by claude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec_internal.h"
#include "../core/core.h"

static int	open_out(char *path, int app)
{
	if (app)
		return (open(path, O_WRONLY | O_CREAT | O_APPEND, 0644));
	return (open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644));
}

static int	open_redirect(t_redirect *r)
{
	if (r->type == REDIRECT_IN)
		return (open(r->target, O_RDONLY));
	if (r->type == REDIRECT_OUT)
		return (open_out(r->target, 0));
	if (r->type == REDIRECT_APPEND)
		return (open_out(r->target, 1));
	return (r->fd);
}

int	apply_redirects(t_shell *sh, t_redirect *r)
{
	int	fd;

	while (r)
	{
		fd = open_redirect(r);
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
