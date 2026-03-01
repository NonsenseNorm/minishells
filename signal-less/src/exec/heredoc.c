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

/*
** rl_event_hook: called by readline on EINTR (after signal interrupts read).
** Sets rl_done=1 to make readline return the current line buffer content.
** With rl_catch_signals=0, readline does NOT clear its buffer on SIGINT,
** so we get the actual typed content back and can distinguish:
**   empty string  → Ctrl+C fired with nothing typed  → cancel heredoc
**   non-empty str → Ctrl+C fired mid-line             → discard and continue
*/
static int	heredoc_event_hook(void)
{
	if (g_sig == SIGINT)
		rl_done = 1;
	return (0);
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

static int	heredoc_step(t_shell *sh, int p1, char *delim, bool quoted)
{
	char	*line;
	int		sig;

	line = readline("> ");
	sig = (g_sig == SIGINT);
	if (sig && line && *line)
	{
		free(line);
		g_sig = 0;
		write(STDOUT_FILENO, "\n", 1);
		rl_on_new_line();
		return (1);
	}
	if (sig || !line || !ft_strcmp(line, delim))
	{
		if (line)
			free(line);
		return (0);
	}
	heredoc_write(sh, p1, line, quoted);
	return (1);
}

static int	heredoc_cleanup(t_shell *sh, int p[2])
{
	rl_event_hook = NULL;
	rl_catch_signals = 1;
	close(p[1]);
	if (g_sig == SIGINT)
		return (sig_set_interactive(), close(p[0]), sh->exit_code = 130, -1);
	sig_set_interactive();
	return (p[0]);
}

/*
** heredoc_fd: reads heredoc lines using readline with rl_catch_signals=0.
**
** rl_catch_signals=0 prevents readline from installing its own SIGINT handler,
** so readline does NOT call rl_cleanup_after_signal() on SIGINT.  The line
** buffer is therefore intact when rl_done=1 fires via heredoc_event_hook.
**
** Ctrl+C behaviour:
**   > (empty) + Ctrl+C  → readline returns ""  → cancel heredoc ($?=130)
**   > word + Ctrl+C     → readline returns word → discard, write \n, continue
** Ctrl+D behaviour (readline emacs-mode defaults):
**   > (empty) + Ctrl+D  → readline returns NULL → end heredoc normally
*/
int	heredoc_fd(t_shell *sh, char *delim, bool quoted)
{
	int	p[2];

	if (pipe(p) != 0)
		return (-1);
	sig_set_heredoc();
	rl_catch_signals = 0;
	rl_event_hook = heredoc_event_hook;
	while (heredoc_step(sh, p[1], delim, quoted))
		;
	return (heredoc_cleanup(sh, p));
}
