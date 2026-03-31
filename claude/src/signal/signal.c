/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/03/26 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "signal.h"

static int	g_echoctl;

static void	sig_handler_interactive(int sig)
{
	g_sig = sig;
	if (g_echoctl)
		write(STDOUT_FILENO, "^C", 2);
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void	sig_init_echoctl(struct termios *term)
{
	g_echoctl = (term->c_lflag & ECHOCTL) != 0;
}

static void	sig_handler_heredoc(int sig)
{
	g_sig = sig;
	close(STDIN_FILENO);
}

void	sig_set_interactive(void)
{
	g_sig = 0;
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, sig_handler_interactive);
}

void	sig_set_heredoc(void)
{
	struct sigaction	sa;

	g_sig = 0;
	signal(SIGQUIT, SIG_IGN);
	sa.sa_handler = sig_handler_heredoc;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
}
