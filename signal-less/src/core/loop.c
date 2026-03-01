/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loop.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claude <claude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/01/01 00:00:00 by claude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ms.h"

static int	handle_line(t_shell *sh, char *line)
{
	t_token		*tok;
	t_pipeline	pl;
	t_mark		mark;

	mem_mark(&sh->mem, &mark);
	tok = NULL;
	if (lex_line(sh, line, &tok) != 0)
		return (mem_pop(&sh->mem, &mark), sh->exit_code);
	if (parse_pipeline(sh, tok, &pl) != 0)
		return (mem_pop(&sh->mem, &mark), sh->exit_code);
	if (expand_pipeline(sh, &pl) != 0)
		return (mem_pop(&sh->mem, &mark), sh->exit_code);
	sh->exit_code = exec_pipeline(sh, &pl);
	mem_pop(&sh->mem, &mark);
	return (sh->exit_code);
}

static char	*read_prompt(t_shell *sh)
{
	char	*prompt;
	char	*line;

	ms_term_disable_echoctl(sh);
	if (sh->interactive)
		prompt = "minishell$ ";
	else
		prompt = "";
	line = readline(prompt);
	return (line);
}

static void	flush_sigint(t_shell *sh)
{
	sh->exit_code = 130;
	g_sig = 0;
	ms_term_disable_echoctl(sh);
}

void	ms_loop(t_shell *sh)
{
	char	*line;

	sig_set_interactive();
	while (1)
	{
		line = read_prompt(sh);
		if (g_sig == SIGINT)
			flush_sigint(sh);
		if (!line)
			break ;
		if (*line)
			add_history(line);
		else
		{
			free(line);
			continue ;
		}
		handle_line(sh, line);
		free(line);
	}
	if (sh->interactive)
		printf("\033[A\033[11Cexit\n");
}
