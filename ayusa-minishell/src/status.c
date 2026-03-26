/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   status.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 16:00:30 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/18 20:27:36 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

// WIFEXITED: Ended Normally, WIFSIGNALED: Terminated by a signal
int	calculate_exit_status(int status)
{
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
		return (EXIT_SIGNAL_BASE + WTERMSIG(status));
	return (status);
}

// calculate_exit_status + Quit print (Determine using WCOREDUMP)
int	calculate_exit_status_quit(int status)
{
	int	term_sig;

	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	if (WIFSIGNALED(status))
	{
		term_sig = WTERMSIG(status);
		if (term_sig == SIGQUIT)
		{
			if (WCOREDUMP(status))
				ft_putstr_fd("Quit (core dumped)\n", STDERR_FILENO);
			else
				ft_putstr_fd("Quit\n", STDERR_FILENO);
		}
		return (EXIT_SIGNAL_BASE + term_sig);
	}
	return (status);
}

void	update_exit_status(t_env **env_list, int status)
{
	char	*status_str;

	status_str = ft_itoa(status);
	if (!status_str)
		return ;
	set_env_value(env_list, "?", status_str);
	free(status_str);
}

// For child processes and built-in exit commands
void	cleanup_and_exit(int status, t_node *ast, t_env *env)
{
	if (ast)
		free_ast(ast);
	if (env)
		free_env(env);
	rl_clear_history();
	exit(status);
}
