/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_fd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claude <claude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/03/27 00:00:00 by claude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "heredoc_internal.h"
#include "../signal/signal.h"

static int	heredoc_loop(t_shell *sh, int wfd, char *delim, bool quoted)
{
	char	*line;

	while (1)
	{
		line = heredoc_read_line();
		if (g_sig == SIGINT)
		{
			if (line)
				free(line);
			write(STDOUT_FILENO, "\n", 1);
			return (1);
		}
		if (!line)
		{
			heredoc_eof_warning(delim);
			return (0);
		}
		if (!ft_strcmp(line, delim))
			return (free(line), 0);
		heredoc_write(sh, wfd, line, quoted);
	}
}

int	heredoc_fd(t_shell *sh, char *delim, bool quoted)
{
	int	p[2];
	int	saved_in;
	int	was_sigint;

	if (pipe(p) != 0)
		return (-1);
	saved_in = dup(STDIN_FILENO);
	sig_set_heredoc();
	was_sigint = heredoc_loop(sh, p[1], delim, quoted);
	close(p[1]);
	if (was_sigint)
		dup2(saved_in, STDIN_FILENO);
	close(saved_in);
	sig_set_interactive();
	if (was_sigint)
		return (close(p[0]), sh->exit_code = 130, -1);
	return (p[0]);
}
