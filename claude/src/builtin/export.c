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
	while (s[i] && s[i] != '=' && !(s[i] == '+' && s[i + 1] == '='))
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

static void	export_append(t_shell *sh, char *key, char *val)
{
	char	*old;
	char	*joined;

	old = env_get(&sh->env, key);
	if (!old)
		return (env_set(&sh->env, key, val, true), (void)0);
	joined = ft_strjoin(old, val);
	if (!joined)
		return ;
	env_set(&sh->env, key, joined, true);
	free(joined);
}

static void	export_one(t_shell *sh, char *arg)
{
	char	*eq;
	char	*key;
	bool	append;

	eq = ft_strchr(arg, '=');
	if (!eq)
		return (env_set(&sh->env, arg, "", true), (void)0);
	append = (eq > arg && *(eq - 1) == '+');
	key = ft_substr(arg, 0, (size_t)(eq - append - arg));
	if (!key)
		return ;
	if (append)
		export_append(sh, key, eq + 1);
	else
		env_set(&sh->env, key, eq + 1, true);
	free(key);
}

int	bi_export(t_shell *sh, t_cmd *cmd)
{
	int	ret;
	int	i;

	if (!cmd->argv[1])
		return (print_export(sh));
	ret = 0;
	i = 1;
	while (cmd->argv[i])
	{
		if (!valid_ident(cmd->argv[i]))
		{
			fprintf(stderr,
				"minishell: export: `%s': not a valid identifier\n",
				cmd->argv[i]);
			ret = 1;
		}
		else
			export_one(sh, cmd->argv[i]);
		i++;
	}
	return (ret);
}
