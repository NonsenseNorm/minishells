/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claude <claude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/01/01 00:00:00 by claude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../core/ms.h"

static void	sig_handler_interactive(int sig)
{
	g_sig = sig;
	write(STDOUT_FILENO, "\n", 1);
	rl_replace_line("", 0);
	if (rl_prompt)
		write(STDOUT_FILENO, rl_prompt, ft_strlen(rl_prompt));
	rl_on_new_line_with_prompt();
}

static void	sig_handler_record(int sig)
{
	g_sig = sig;
}

/*
** Interactive mode: SIGQUIT ignored, SIGINT clears the buffer and sets
** rl_done=1 so readline returns "" immediately; g_sig is reset by the loop.
*/
void	sig_set_interactive(void)
{
	g_sig = 0;
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, sig_handler_interactive);
}

/*
** Execution (parent): both signals ignored so the parent keeps waiting.
** Children in the same process group receive the signals naturally.
*/
void	sig_set_exec_parent(void)
{
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, SIG_IGN);
}

/*
** Heredoc mode: SIGQUIT ignored, SIGINT records the signal so the
** heredoc reading loop can detect cancellation via g_sig == SIGINT.
** SA_RESTART is NOT set so that read() returns EINTR immediately,
** allowing the heredoc loop to detect the signal without delay.
*/
void	sig_set_heredoc(void)
{
	struct sigaction	sa;

	g_sig = 0;
	signal(SIGQUIT, SIG_IGN);
	sa.sa_handler = sig_handler_record;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
}
