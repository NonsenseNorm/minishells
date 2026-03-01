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

static int	line_matches(char *line, char *delim)
{
	return (!ft_strcmp(line, delim));
}

int	heredoc_fd(t_shell *sh, char *delim, bool quoted)
{
	int		p[2];
	char	*line;
	char	*tmp;

	if (pipe(p) != 0)
		return (-1);
	sig_set_heredoc();
	while (1)
	{
		if (isatty(STDIN_FILENO))
			write(1, "> ", 2);
		line = read_heredoc_line();
		if (!line || g_sig == SIGINT || line_matches(line, delim))
			break ;
		if (!quoted)
		{
			tmp = expand_word(sh, line);
			if (tmp)
				(free(line), line = tmp);
		}
		write(p[1], line, ft_strlen(line));
		write(p[1], "\n", 1);
		free(line);
	}
	if (line)
		free(line);
	close(p[1]);
	sig_set_interactive();
	if (g_sig == SIGINT)
		return (close(p[0]), sh->exit_code = 130, -1);
	return (p[0]);
}
