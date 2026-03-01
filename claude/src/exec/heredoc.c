/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claude <claude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/01/01 00:00:00 by claude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../core/ms.h"

static char	*read_heredoc_line(void)
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

static void	heredoc_write(t_shell *sh, int fd, char *line, bool quoted)
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

int	heredoc_fd(t_shell *sh, char *delim, bool quoted)
{
	int		p[2];
	char	*line;

	if (pipe(p) != 0)
		return (-1);
	sig_set_heredoc();
	while (1)
	{
		if (isatty(STDIN_FILENO))
			write(1, "> ", 2);
		line = read_heredoc_line();
		if (!line || g_sig == SIGINT || !ft_strcmp(line, delim))
			break ;
		heredoc_write(sh, p[1], line, quoted);
		line = NULL;
	}
	if (line)
		free(line);
	close(p[1]);
	sig_set_interactive();
	if (g_sig == SIGINT)
		return (close(p[0]), sh->exit_code = 130, -1);
	return (p[0]);
}
