/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_fd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/03/27 00:00:00 by stanizak         ###   ########.fr       */
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

static char	*heredoc_tmppath(void)
{
	unsigned char	buf[16];
	char			hex[33];
	int				fd;
	int				i;

	fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0)
		return (NULL);
	if (read(fd, buf, 16) != 16)
		return (close(fd), NULL);
	close(fd);
	i = 0;
	while (i < 16)
	{
		hex[i * 2] = "0123456789abcdef"[buf[i] >> 4];
		hex[i * 2 + 1] = "0123456789abcdef"[buf[i] & 0x0f];
		i++;
	}
	hex[32] = '\0';
	return (ft_strjoin("/tmp/.minishell_heredoc_", hex));
}

int	heredoc_fd(t_shell *sh, char *delim, bool quoted)
{
	char	*path;
	int		fd;
	int		saved_in;
	int		was_sigint;

	path = heredoc_tmppath();
	if (!path)
		return (-1);
	fd = open(path, O_CREAT | O_WRONLY | O_EXCL, 0600);
	if (fd < 0)
		return (free(path), -1);
	saved_in = dup(STDIN_FILENO);
	sig_set_heredoc();
	was_sigint = heredoc_loop(sh, fd, delim, quoted);
	close(fd);
	if (was_sigint)
		dup2(saved_in, STDIN_FILENO);
	close(saved_in);
	sig_set_interactive();
	if (was_sigint)
		return (unlink(path), free(path), sh->exit_code = 130, -1);
	fd = open(path, O_RDONLY);
	unlink(path);
	free(path);
	return (fd);
}
