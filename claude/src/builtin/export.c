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
#include "../core/core.h"

static void	sort_idx(int *idx, t_var *vars, int len)
{
	int	i;
	int	j;
	int	tmp;

	i = -1;
	while (++i < len)
		idx[i] = i;
	i = -1;
	while (++i < len - 1)
	{
		j = i;
		while (++j < len)
		{
			if (ft_strcmp(vars[idx[i]].key, vars[idx[j]].key) > 0)
			{
				tmp = idx[i];
				idx[i] = idx[j];
				idx[j] = tmp;
			}
		}
	}
}

static int	print_export(t_shell *sh)
{
	int	*idx;
	int	i;
	int	v;

	idx = malloc(sizeof(int) * sh->env.len);
	if (!idx)
		return (1);
	sort_idx(idx, sh->env.vars, sh->env.len);
	i = -1;
	while (++i < sh->env.len)
	{
		v = idx[i];
		if (sh->env.vars[v].val)
			printf("declare -x %s=\"%s\"\n", sh->env.vars[v].key,
				sh->env.vars[v].val);
		else
			printf("declare -x %s\n", sh->env.vars[v].key);
	}
	return (free(idx), 0);
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
		return (env_set(&sh->env, arg, NULL, true), (void)0);
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
			ms_err("minishell: export: `");
			ms_err(cmd->argv[i]);
			ms_err("': not a valid identifier\n");
			ret = 1;
		}
		else
			export_one(sh, cmd->argv[i]);
		i++;
	}
	return (ret);
}
