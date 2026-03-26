/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_line.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 10:58:36 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/21 20:24:27 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static void		exec_line(char *line, t_env **env_list);
static int		is_only_spaces(char *s);
static t_node	*build_ast(char *line, t_env **env_list);

// If bracket paste returns multiple lines, split and execute each
void	exec_multiline(char *line, t_env **env_list)
{
	char	**lines;
	int		i;

	if (!line || !*line)
		return ;
	if (!ft_strchr(line, '\n'))
	{
		exec_line(line, env_list);
		return ;
	}
	lines = ft_split_c(line, '\n');
	if (!lines)
		return ;
	i = 0;
	while (lines[i])
	{
		exec_line(lines[i], env_list);
		i++;
	}
	free_str_array(lines);
}

static void	exec_line(char *line, t_env **env_list)
{
	t_node	*node;

	if (!line || !*line || is_only_spaces(line))
		return ;
	node = build_ast(line, env_list);
	if (!node)
	{
		update_exit_status(env_list, EXIT_SYNTAX_ERR);
		return ;
	}
	set_signal_executing();
	exec_ast(node, env_list, node);
	if (g_sig == SIGINT)
	{
		ft_putstr_fd("\n", STDERR_FILENO);
		g_sig = 0;
	}
	free_ast(node);
}

static int	is_only_spaces(char *s)
{
	while (*s)
	{
		if (*s != ' ' && *s != '\t')
			return (0);
		s++;
	}
	return (1);
}

// lexer -> parser -> process_heredoc -> return root AST
static t_node	*build_ast(char *line, t_env **env_list)
{
	t_token	*tokens;
	t_token	*head;
	t_node	*node;

	tokens = tokenize(line);
	if (!tokens)
		return (NULL);
	head = tokens;
	node = parse(&tokens);
	free_tokens(head);
	if (!node)
		return (NULL);
	if (process_heredoc(node, *env_list))
	{
		free_ast(node);
		return (NULL);
	}
	return (node);
}
