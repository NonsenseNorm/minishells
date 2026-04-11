/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/04/11 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expand_internal.h"
#include "../mem/mem.h"
#include "../parser/parser.h"

static int	count_exp_argc(t_shell *sh, t_cmd *cmd)
{
	int		i;
	int		n;
	char	*e;

	i = 0;
	n = 0;
	while (cmd->argv && cmd->argv[i])
	{
		e = expand_word(sh, cmd->argv[i]);
		if (!e)
			return (-1);
		if (*e != '\0' || has_quote(cmd->argv[i]))
		{
			if (has_quote(cmd->argv[i]))
				n++;
			else
				n += ifs_wc(e);
		}
		free(e);
		i++;
	}
	return (n);
}

static int	add_split(t_mem *mem, char *exp, t_exp_ctx *ctx)
{
	char	**flds;
	int		i;

	flds = split_ifs(exp);
	if (!flds)
		return (1);
	i = 0;
	while (flds[i])
	{
		ctx->nav[ctx->dst] = ms_strdup(mem, flds[i]);
		if (!ctx->nav[ctx->dst])
			return (ft_free_split(flds), 1);
		ctx->dst++;
		i++;
	}
	ft_free_split(flds);
	return (0);
}

static int	fill_one(t_shell *sh, t_mem *mem, char *orig, t_exp_ctx *ctx)
{
	char	*exp;
	int		ret;

	exp = expand_word(sh, orig);
	if (!exp)
		return (1);
	if (*exp == '\0' && !has_quote(orig))
		return (free(exp), 0);
	if (!has_quote(orig))
	{
		ret = add_split(mem, exp, ctx);
		free(exp);
		return (ret);
	}
	ctx->nav[ctx->dst] = ms_strdup(mem, exp);
	free(exp);
	if (!ctx->nav[ctx->dst])
		return (1);
	ctx->dst++;
	return (0);
}

static int	expand_cmd_argv(t_shell *sh, t_mem *mem, t_cmd *cmd)
{
	int			total;
	t_exp_ctx	ctx;
	int			i;

	total = count_exp_argc(sh, cmd);
	if (total < 0)
		return (1);
	ctx.nav = ms_alloc(mem, sizeof(char *) * (total + 1));
	if (!ctx.nav)
		return (1);
	ctx.dst = 0;
	i = 0;
	while (cmd->argv && cmd->argv[i])
	{
		if (fill_one(sh, mem, cmd->argv[i], &ctx))
			return (1);
		i++;
	}
	ctx.nav[ctx.dst] = NULL;
	cmd->argv = ctx.nav;
	return (0);
}

int	expand_pipeline(t_shell *sh, t_mem *mem, t_pipeline *pl)
{
	t_cmd	*new_cmds;
	int		i;

	new_cmds = ms_alloc(mem, sizeof(t_cmd) * pl->count);
	if (!new_cmds)
		return (1);
	ft_memcpy(new_cmds, pl->cmds, sizeof(t_cmd) * pl->count);
	pl->cmds = new_cmds;
	i = 0;
	while (i < pl->count)
	{
		if (expand_cmd_argv(sh, mem, &pl->cmds[i]) != 0)
			return (1);
		if (expand_cmd_redirects(sh, mem, &pl->cmds[i]) != 0)
			return (1);
		i++;
	}
	return (0);
}
