/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/02/24 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../core/minishell.h"

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

void	sig_set_interactive(void)
{
	g_sig = 0;
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, sig_handler_interactive);
}

void	sig_set_exec_parent(void)
{
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, SIG_IGN);
}

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
