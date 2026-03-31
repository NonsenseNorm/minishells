/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_interactive.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/04/01 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "signal.h"

static int	echoctl_flag(int set, int val)
{
	static int	echoctl;

	if (set)
		echoctl = val;
	return (echoctl);
}

static void	sig_handler_interactive(int sig)
{
	g_sig = sig;
	if (echoctl_flag(0, 0))
		write(STDOUT_FILENO, "^C", 2);
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void	sig_set_interactive(void)
{
	struct termios	term;

	g_sig = 0;
	tcgetattr(STDIN_FILENO, &term);
	echoctl_flag(1, (term.c_lflag & ECHOCTL) != 0);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, sig_handler_interactive);
}
