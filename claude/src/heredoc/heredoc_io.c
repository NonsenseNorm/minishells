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
	t_strbuf	sb;
	char		c;
	ssize_t		r;

	sb_init(&sb);
	while (1)
	{
		r = read(STDIN_FILENO, &c, 1);
		if (r <= 0)
			break ;
		if (c == '\n')
			break ;
		if (sb_append(&sb, &c, 1) < 0)
			return (NULL);
	}
	if (r <= 0 && sb.len == 0)
		return (free(sb.buf), NULL);
	return (sb_detach(&sb));
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
		tmp = expand_heredoc_line(sh, line);
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
