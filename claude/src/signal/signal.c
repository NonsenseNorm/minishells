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
	rl_on_new_line();
	write(STDOUT_FILENO, "\n", 1);
	rl_replace_line("", 0);
	rl_done = 1;
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
** Execution (child): restore defaults so the child can be killed normally.
*/
void	sig_set_exec_child(void)
{
	signal(SIGQUIT, SIG_DFL);
	signal(SIGINT, SIG_DFL);
}

/*
** Heredoc mode: SIGQUIT ignored, SIGINT records the signal so the
** heredoc reading loop can detect cancellation via g_sig == SIGINT.
*/
void	sig_set_heredoc(void)
{
	g_sig = 0;
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, sig_handler_record);
}
