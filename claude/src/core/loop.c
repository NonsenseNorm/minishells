/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loop.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/03/30 23:05:34 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "core_internal.h"
#include "../mem/mem.h"
#include "../lexer/lexer.h"
#include "../parser/parser.h"
#include "../expand/expand.h"
#include "../exec/exec.h"
#include "../signal/signal.h"

int	ms_loop(t_shell *sh, char *line)
{
	t_token		*tok;
	t_pipeline	pl;
	t_mem		lex_mem;
	t_mem		parse_mem;
	t_mem		exp_mem;

	mem_init(&lex_mem);
	tok = NULL;
	if (lex_line(sh, &lex_mem, line, &tok) != 0)
		return (mem_reset(&lex_mem), sh->exit_code);
	mem_init(&parse_mem);
	if (parse_pipeline(sh, &parse_mem, tok, &pl) != 0)
		return (mem_reset(&lex_mem), mem_reset(&parse_mem), sh->exit_code);
	mem_reset(&lex_mem);
	mem_init(&exp_mem);
	if (expand_pipeline(sh, &exp_mem, &pl) != 0)
		return (mem_reset(&parse_mem), mem_reset(&exp_mem),
			sh->exit_code = 1);
	mem_reset(&parse_mem);
	sh->exit_code = exec_pipeline(sh, &pl);
	mem_reset(&exp_mem);
	return (sh->exit_code);
}

static char	*read_prompt(t_shell *sh)
{
	char	*line;

	if (sh->interactive)
		line = readline("minishell$ ");
	else
		line = get_next_line(STDIN_FILENO);
	return (line);
}

static void	process_input(t_shell *sh, char *input)
{
	char	*ptr;
	char	*nl;

	ptr = input;
	nl = ft_strchr(ptr, '\n');
	while (nl != NULL)
	{
		*nl = '\0';
		if (*ptr)
		{
			add_history(ptr);
			ms_loop(sh, ptr);
		}
		ptr = nl + 1;
		nl = ft_strchr(ptr, '\n');
	}
	if (*ptr)
	{
		add_history(ptr);
		ms_loop(sh, ptr);
	}
}

static void	print_exit(t_shell *sh)
{
	if (!sh->interactive)
		return ;
	write(STDOUT_FILENO, "\033[2K\r", 5);
	printf("exit\n");
}

void	ms_run(t_shell *sh)
{
	char	*input;

	while (1)
	{
		if (sh->interactive)
			sig_set_interactive();
		else
			sig_set_noninteractive();
		input = read_prompt(sh);
		if (g_sig == SIGINT)
		{
			sh->exit_code = 130;
			g_sig = 0;
		}
		if (!input)
			break ;
		if (!*input)
		{
			free(input);
			continue ;
		}
		process_input(sh, input);
		free(input);
	}
	print_exit(sh);
}
