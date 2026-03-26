/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_external.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 17:30:58 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/21 19:29:43 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/minishell.h"

static void	exec_child(char **args, t_env **env_list, t_node *root);
static void	child_cmd_not_found(char **args, t_env **env_list, t_node *root);
static void	child_exec_failed(char **args, char *path, char **envp);

int	exec_external(char **args, t_env **env_list, t_node *root)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid < 0)
	{
		perror("minishell: fork");
		return (1);
	}
	if (pid == 0)
		exec_child(args, env_list, root);
	waitpid(pid, &status, 0);
	return (calculate_exit_status_quit(status));
}

// child process
static void	exec_child(char **args, t_env **env_list, t_node *root)
{
	char	*path;
	char	**envp;

	set_signal_child();
	path = get_cmd_path(args[0], *env_list);
	if (!path)
		child_cmd_not_found(args, env_list, root);
	envp = env_list_to_array(*env_list);
	execve(path, args, envp);
	child_exec_failed(args, path, envp);
	cleanup_and_exit(EXIT_NOT_EXEC, root, *env_list);
}

// cmd not found error
static void	child_cmd_not_found(char **args, t_env **env_list, t_node *root)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd(args[0], STDERR_FILENO);
	if (ft_strchr(args[0], '/')
		|| !get_env_value(*env_list, "PATH"))
		ft_putstr_fd(": No such file or directory\n", STDERR_FILENO);
	else
		ft_putstr_fd(": command not found\n", STDERR_FILENO);
	cleanup_and_exit(EXIT_NOT_FOUND, root, *env_list);
}

// execve failed error
static void	child_exec_failed(char **args, char *path, char **envp)
{
	struct stat	st;

	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd(args[0], STDERR_FILENO);
	if (stat(path, &st) == 0 && S_ISDIR(st.st_mode))
		ft_putstr_fd(": Is a directory\n", STDERR_FILENO);
	else
		ft_putstr_fd(": Permission denied\n", STDERR_FILENO);
	free(path);
	free_str_array(envp);
}
