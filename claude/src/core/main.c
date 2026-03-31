/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/03/30 23:05:17 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "core_internal.h"
#include "../env/env.h"
#include "../signal/signal.h"
#include "../term/term.h"

volatile sig_atomic_t	g_sig;

static int	init_shell(t_shell *sh, char **envp)
{
	rl_catch_signals = 0;
	sh->exit_code = 0;
	sh->interactive = isatty(STDIN_FILENO);
	term_save(sh);
	if (env_init(&sh->env, envp) != 0)
		return (1);
	return (0);
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	sh;

	(void)argc;
	(void)argv;
	if (init_shell(&sh, envp) != 0)
		return (1);
	ms_run(&sh);
	term_restore(&sh);
	rl_clear_history();
	env_free(&sh.env);
	return (sh.exit_code);
}
