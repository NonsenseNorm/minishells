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

static void	heredoc_eof_warning(char *delim)
{
	write(STDERR_FILENO, "\nminishell: warning: here-document delimited"
		" by end-of-file (wanted `", 69);
	write(STDERR_FILENO, delim, ft_strlen(delim));
	write(STDERR_FILENO, "')\n", 3);
}

int	heredoc_fd(t_shell *sh, char *delim, bool quoted)
{
	int		p[2];
	char	*line;
	int		was_sigint;

	if (pipe(p) != 0)
		return (-1);
	sig_set_heredoc();
	while (1)
	{
		if (isatty(STDIN_FILENO))
			write(1, "> ", 2);
		line = read_heredoc_line();
		if (g_sig == SIGINT)
		{
			if (line)
				free(line);
			write(STDOUT_FILENO, "^C\n", 3);
			break ;
		}
		if (!line)
		{
			heredoc_eof_warning(delim);
			break ;
		}
		if (!ft_strcmp(line, delim))
		{
			free(line);
			break ;
		}
		heredoc_write(sh, p[1], line, quoted);
		line = NULL;
	}
	was_sigint = (g_sig == SIGINT);
	close(p[1]);
	sig_set_interactive();
	if (was_sigint)
		return (close(p[0]), sh->exit_code = 130, -1);
	return (p[0]);
}

void	close_pipeline_heredocs(t_pipeline *pl)
{
	t_redirect	*r;
	int			i;

	i = 0;
	while (i < pl->count)
	{
		r = pl->cmds[i].redirects;
		while (r)
		{
			if (r->type == REDIRECT_HEREDOC && r->fd >= 0)
			{
				close(r->fd);
				r->fd = -1;
			}
			r = r->next;
		}
		i++;
	}
}

int	gather_heredocs(t_shell *sh, t_pipeline *pl)
{
	t_redirect	*r;
	int			i;

	i = 0;
	while (i < pl->count)
	{
		r = pl->cmds[i].redirects;
		while (r)
		{
			if (r->type == REDIRECT_HEREDOC)
			{
				r->fd = heredoc_fd(sh, r->target, r->quoted);
				if (r->fd < 0)
				{
					close_pipeline_heredocs(pl);
					return (-1);
				}
			}
			r = r->next;
		}
		i++;
	}
	return (0);
}
