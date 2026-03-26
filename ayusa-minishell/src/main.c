/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:11:59 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/21 19:17:00 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

volatile sig_atomic_t	g_sig = 0;

static void	read_prompt(t_env **env_list);

// Save terminal information -> env list init -> read prompt -> status return
int	main(int argc, char **argv, char **envp)
{
	t_env			*env_list;
	int				exit_status;

	(void)argc;
	(void)argv;
	rl_catch_signals = 0;
	env_list = env_init(envp);
	read_prompt(&env_list);
	rl_clear_history();
	exit_status = ft_atoi(get_env_value(env_list, "?"));
	free_env(env_list);
	return (exit_status);
}

// read prompt -> exec_multiline
static void	read_prompt(t_env **env_list)
{
	char	*line;

	while (1)
	{
		set_signal_interactive();
		line = readline(PROMPT);
		if (line == NULL)
		{
			ft_putendl_fd("exit", STDERR_FILENO);
			break ;
		}
		if (g_sig == SIGINT)
		{
			update_exit_status(env_list, EXIT_SIGINT);
			g_sig = 0;
		}
		if (*line != '\0')
			add_history(line);
		exec_multiline(line, env_list);
		free(line);
	}
}
