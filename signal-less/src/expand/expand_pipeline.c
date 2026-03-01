/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_pipeline.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claude <claude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/01/01 00:00:00 by claude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../core/ms.h"

static int	expand_argv(t_shell *sh, t_cmd *cmd)
{
	int		i;
	char	*neww;

	i = 0;
	while (cmd->argv && cmd->argv[i])
	{
		neww = expand_word(sh, cmd->argv[i]);
		if (!neww)
			return (1);
		cmd->argv[i] = ms_strdup(&sh->mem, neww);
		free(neww);
		i++;
	}
	return (0);
}

static int	expand_redirects(t_shell *sh, t_redirect *r)
{
	char	*tmp;

	while (r)
	{
		if (r->type == REDIRECT_HEREDOC)
			tmp = strip_quotes(r->target);
		else
			tmp = expand_word(sh, r->target);
		if (!tmp)
			return (1);
		r->target = ms_strdup(&sh->mem, tmp);
		free(tmp);
		r = r->next;
	}
	return (0);
}

int	expand_pipeline(t_shell *sh, t_pipeline *pl)
{
	int	i;

	i = 0;
	while (i < pl->count)
	{
		if (expand_argv(sh, &pl->cmds[i]) != 0)
			return (1);
		if (expand_redirects(sh, pl->cmds[i].redirects) != 0)
			return (1);
		i++;
	}
	return (0);
}
