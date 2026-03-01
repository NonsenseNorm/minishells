/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 00:00:00 by user              #+#    #+#             */
/*   Updated: 2026/02/24 21:28:08 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_sig = 0;

static int	init_shell(t_shell *sh, char **envp)
{
	sh->exit_code = 0;
	sh->interactive = isatty(STDIN_FILENO);
	if (env_init(&sh->env, envp) != 0)
		return (1);
	mem_init(&sh->mem);
	if (!sh->mem.top)
	{
		env_free(&sh->env);
		return (1);
	}
	return (0);
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	sh;

	(void)argc;
	(void)argv;
	if (init_shell(&sh, envp) != 0)
		return (1);
	ms_loop(&sh);
	clear_history();
	env_free(&sh.env);
	mem_reset(&sh.mem);
	return (sh.exit_code);
}
