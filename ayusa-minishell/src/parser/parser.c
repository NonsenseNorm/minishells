/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:11:49 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/20 23:58:19 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static void			print_syntax_error(char *token_val);
static t_node_type	get_list_type(t_token_type type);
static t_node		*parse_pipeline(t_token **tokens);

// Entrance -> parse_list
t_node	*parse(t_token **tokens)
{
	t_node	*root;

	if (!tokens || !*tokens)
		return (NULL);
	root = parse_list(tokens);
	if (root == NULL)
	{
		if (*tokens)
			print_syntax_error((*tokens)->value);
		else
			print_syntax_error(NULL);
		return (NULL);
	}
	if (*tokens != NULL)
	{
		print_syntax_error((*tokens)->value);
		free_ast(root);
		return (NULL);
	}
	return (root);
}

static void	print_syntax_error(char *token_val)
{
	ft_putstr_fd("minishell: syntax error near ", STDERR_FILENO);
	ft_putstr_fd("unexpected token `", STDERR_FILENO);
	if (token_val)
		ft_putstr_fd(token_val, STDERR_FILENO);
	else
		ft_putstr_fd("newline", STDERR_FILENO);
	ft_putstr_fd("'\n", STDERR_FILENO);
}

// &&, || is Lowest priority -> parse_pipeline
t_node	*parse_list(t_token **tokens)
{
	t_node		*node;
	t_node		*right;
	t_node_type	type;

	node = parse_pipeline(tokens);
	if (!node)
		return (NULL);
	while (*tokens && ((*tokens)->type == TK_AND
			|| (*tokens)->type == TK_OR))
	{
		type = get_list_type((*tokens)->type);
		*tokens = (*tokens)->next;
		right = parse_pipeline(tokens);
		if (!right)
		{
			free_ast(node);
			return (NULL);
		}
		node = new_node(type, node, right);
	}
	return (node);
}

static t_node_type	get_list_type(t_token_type type)
{
	if (type == TK_AND)
		return (NODE_AND);
	return (NODE_OR);
}

// Next, process the lower-priority pipe -> parse_command
static t_node	*parse_pipeline(t_token **tokens)
{
	t_node	*node;
	t_node	*right;

	node = parse_command(tokens);
	if (!node)
		return (NULL);
	while (*tokens && (*tokens)->type == TK_PIPE)
	{
		*tokens = (*tokens)->next;
		right = parse_command(tokens);
		if (!right)
		{
			free_ast(node);
			return (NULL);
		}
		node = new_node(NODE_PIPE, node, right);
	}
	return (node);
}
