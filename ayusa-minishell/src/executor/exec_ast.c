/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_ast.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:11:00 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/18 20:16:01 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static int	exec_and(t_node *node, t_env **env_list, t_node *root);
static int	exec_or(t_node *node, t_env **env_list, t_node *root);
static int	exec_subshell(t_node *node, t_env **env_list, t_node *root);

// AST -> assign &&, ||, ()
int	exec_ast(t_node *node, t_env **env_list, t_node *root)
{
	int	status;

	status = 0;
	if (node == NULL)
		return (0);
	if (node->type == NODE_CMD)
		status = exec_cmd(node, env_list, root);
	else if (node->type == NODE_PIPE)
		status = exec_pipeline(node, env_list, root);
	else if (node->type == NODE_AND)
		status = exec_and(node, env_list, root);
	else if (node->type == NODE_OR)
		status = exec_or(node, env_list, root);
	else if (node->type == NODE_SUBSHELL)
		status = exec_subshell(node, env_list, root);
	update_exit_status(env_list, status);
	return (status);
}

// &&
static int	exec_and(t_node *node, t_env **env_list, t_node *root)
{
	int	status;

	if (!node)
		return (0);
	status = exec_ast(node->left, env_list, root);
	update_exit_status(env_list, status);
	if (status == 0)
	{
		status = exec_ast(node->right, env_list, root);
		update_exit_status(env_list, status);
	}
	return (status);
}

// ||
static int	exec_or(t_node *node, t_env **env_list, t_node *root)
{
	int	status;

	if (!node)
		return (0);
	status = exec_ast(node->left, env_list, root);
	update_exit_status(env_list, status);
	if (status != 0)
	{
		status = exec_ast(node->right, env_list, root);
		update_exit_status(env_list, status);
	}
	return (status);
}

// sub shell
static int	exec_subshell(t_node *node, t_env **env_list, t_node *root)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid < 0)
		return (1);
	if (pid == 0)
	{
		status = exec_ast(node->left, env_list, root);
		cleanup_and_exit(status, root, *env_list);
	}
	waitpid(pid, &status, 0);
	return (calculate_exit_status(status));
}
