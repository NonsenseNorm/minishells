/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_io.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/03/29 00:44:25 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "heredoc_internal.h"
#include "../expand/expand.h"

static char	*heredoc_read_canonical(void)
{
	char	buf[2];
	char	*line;
	char	*tmp;
	ssize_t	r;

	line = ft_strdup("");
	if (!line)
		return (NULL);
	while (1)
	{
		r = read(STDIN_FILENO, buf, 1);
		if (r <= 0)
			break ;
		buf[1] = 0;
		if (buf[0] == '\n')
			break ;
		tmp = ft_strjoin(line, buf);
		free(line);
		line = tmp;
		if (!line)
			return (NULL);
	}
	if (r <= 0 && !*line)
		return (free(line), NULL);
	return (line);
}

char	*heredoc_read_line(void)
{
	if (!isatty(STDIN_FILENO))
		return (heredoc_read_canonical());
	return (readline("> "));
}

void	heredoc_write(t_shell *sh, int fd, char *line, bool quoted)
{
	char	*tmp;

	if (!quoted)
	{
		tmp = expand_word(sh, line);
		if (tmp)
		{
			free(line);
			line = tmp;
		}
	}
	write(fd, line, ft_strlen(line));
	write(fd, "\n", 1);
	free(line);
}

void	heredoc_eof_warning(char *delim)
{
	write(STDERR_FILENO, "minishell: warning: here-document delimited"
		" by end-of-file (wanted `", 68);
	write(STDERR_FILENO, delim, ft_strlen(delim));
	write(STDERR_FILENO, "')\n", 3);
}
