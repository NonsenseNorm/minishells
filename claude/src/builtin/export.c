/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/03/28 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin_internal.h"
#include "../env/env.h"

int	valid_ident(const char *s)
{
	int	i;

	if (!s || !(ft_isalpha(*s) || *s == '_'))
		return (0);
	i = 1;
	while (s[i] && s[i] != '=')
	{
		if (!(ft_isalnum(s[i]) || s[i] == '_'))
			return (0);
		i++;
	}
	return (1);
}

static int	print_export(t_shell *sh)
{
	int	i;

	i = 0;
	while (i < sh->env.len)
	{
		printf("declare -x %s\n", sh->env.arr[i]);
		i++;
	}
	return (0);
}

static void	export_one(t_shell *sh, char *arg)
{
	char	*eq;
	char	*key;

	eq = ft_strchr(arg, '=');
	if (!eq)
		env_set(&sh->env, arg, "", true);
	else
	{
		key = ft_substr(arg, 0, (size_t)(eq - arg));
		if (!key)
			return ;
		env_set(&sh->env, key, eq + 1, true);
		free(key);
	}
}

int	bi_export(t_shell *sh, t_cmd *cmd)
{
	int	i;

	if (!cmd->argv[1])
		return (print_export(sh));
	i = 1;
	while (cmd->argv[i])
	{
		if (!valid_ident(cmd->argv[i]))
			return (fprintf(stderr,
					"minishell: export: `%s': not a valid identifier\n",
					cmd->argv[i]), 1);
		export_one(sh, cmd->argv[i]);
		i++;
	}
	return (0);
}
