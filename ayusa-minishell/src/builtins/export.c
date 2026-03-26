/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:10:26 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/18 20:13:51 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

int			builtin_export(char **args, t_env **env_list);
static int	process_export_arg(char *arg, t_env **env_list);
static int	is_valid_identifier(char *name);
static int	export_error(char *arg);

int	builtin_export(char **args, t_env **env_list)
{
	int	i;
	int	ret;

	if (!args[1])
	{
		print_exported_env(*env_list);
		return (0);
	}
	ret = 0;
	i = 1;
	while (args[i])
	{
		if (process_export_arg(args[i], env_list))
			ret = 1;
		i++;
	}
	return (ret);
}

static int	process_export_arg(char *arg, t_env **env_list)
{
	char	*sep;
	char	*key;

	sep = ft_strchr(arg, '=');
	if (sep)
	{
		key = ft_substr(arg, 0, sep - arg);
		if (!is_valid_identifier(key))
		{
			free(key);
			return (export_error(arg));
		}
		set_env_value(env_list, key, sep + 1);
		free(key);
	}
	else
	{
		if (!is_valid_identifier(arg))
			return (export_error(arg));
		if (!get_env_value(*env_list, arg))
			set_env_value(env_list, arg, NULL);
	}
	return (0);
}

static int	is_valid_identifier(char *name)
{
	int	i;

	if (!name || !name[0])
		return (0);
	if (!ft_isalpha(name[0]) && name[0] != '_')
		return (0);
	i = 1;
	while (name[i])
	{
		if (!ft_isalnum(name[i]) && name[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

static int	export_error(char *arg)
{
	ft_putstr_fd("minishell: export: `", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
	return (1);
}
