/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:11:55 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/18 20:27:10 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

static void	handler_executing(int signum);

// A: Waiting for input at the prompt
void	set_signal_interactive(void)
{
	signal(SIGINT, handler_interactive);
	signal(SIGQUIT, SIG_IGN);
}

void	handler_interactive(int signum)
{
	g_sig = signum;
	write(STDOUT_FILENO, "^C\n", 3);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

// B: Command executing (child process)

// Parent process
void	set_signal_executing(void)
{
	signal(SIGINT, handler_executing);
	signal(SIGQUIT, SIG_IGN);
}

static void	handler_executing(int signum)
{
	g_sig = signum;
}

// Child process
void	set_signal_child(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}
