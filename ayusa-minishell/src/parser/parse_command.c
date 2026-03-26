/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_command.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 15:37:14 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/18 20:24:03 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static t_node		*parse_subshell(t_token **tokens);
static int			parse_redirect(t_token **tokens, t_node *node);
static void			append_redirect(t_node *node, t_token_type type,
						char *filename);
static t_redirect	*new_redirect(t_token_type type, char *filename);

// Highest priority : single command or sub shell
t_node	*parse_command(t_token **tokens)
{
	t_node	*node;

	if (!tokens || !*tokens)
		return (NULL);
	if ((*tokens)->type == TK_LPAREN)
		return (parse_subshell(tokens));
	node = new_node(NODE_CMD, NULL, NULL);
	if (!node)
		return (NULL);
	while (*tokens && ((*tokens)->type == TK_WORD
			|| is_redirect((*tokens)->type)))
	{
		if ((*tokens)->type == TK_WORD)
		{
			append_arg(node, (*tokens)->value);
			*tokens = (*tokens)->next;
		}
		else if (parse_redirect(tokens, node) == -1)
			return (free_ast(node), NULL);
	}
	if (node->args == NULL && node->redirects == NULL)
		return (free_ast(node), NULL);
	return (node);
}

static t_node	*parse_subshell(t_token **tokens)
{
	t_node	*node;

	*tokens = (*tokens)->next;
	node = parse_list(tokens);
	if (!node)
		return (NULL);
	if (!*tokens || (*tokens)->type != TK_RPAREN)
	{
		free_ast(node);
		return (NULL);
	}
	*tokens = (*tokens)->next;
	return (new_node(NODE_SUBSHELL, node, NULL));
}

static int	parse_redirect(t_token **tokens, t_node *node)
{
	t_token_type	redir_type;

	redir_type = (*tokens)->type;
	*tokens = (*tokens)->next;
	if (!*tokens || (*tokens)->type != TK_WORD)
		return (-1);
	append_redirect(node, redir_type, (*tokens)->value);
	*tokens = (*tokens)->next;
	return (0);
}

// Append a t_redirect to the node
static void	append_redirect(t_node *node, t_token_type type,
		char *filename)
{
	t_redirect	*new_redir;
	t_redirect	*current;

	new_redir = new_redirect(type, filename);
	if (!new_redir)
		return ;
	if (node->redirects == NULL)
	{
		node->redirects = new_redir;
		return ;
	}
	current = node->redirects;
	while (current->next != NULL)
		current = current->next;
	current->next = new_redir;
}

// Create a new t_redirect
static t_redirect	*new_redirect(t_token_type type, char *filename)
{
	t_redirect	*redir;
	int			len;

	redir = (t_redirect *)malloc(sizeof(t_redirect));
	if (!redir)
		return (NULL);
	redir->type = type;
	redir->quoted = 0;
	redir->next = NULL;
	if (type == TK_HEREDOC
		&& (filename[0] == '\'' || filename[0] == '"'))
	{
		redir->quoted = 1;
		len = ft_strlen(filename);
		redir->filename = ft_substr(filename, 1, len - 2);
	}
	else
		redir->filename = ft_strdup(filename);
	return (redir);
}
