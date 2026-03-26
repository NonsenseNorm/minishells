/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_pipeline.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claude <claude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/03/26 00:00:00 by claude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../core/ms.h"

static int	expand_argv(t_shell *sh, t_mem *mem, t_cmd *cmd)
{
	int		argc;
	char	**new_argv;
	char	*tmp;

	argc = 0;
	while (cmd->argv && cmd->argv[argc])
		argc++;
	new_argv = ms_alloc(mem, sizeof(char *) * (argc + 1));
	if (!new_argv)
		return (1);
	new_argv[argc] = NULL;
	argc = 0;
	while (cmd->argv && cmd->argv[argc])
	{
		tmp = expand_word(sh, cmd->argv[argc]);
		if (!tmp)
			return (1);
		new_argv[argc] = ms_strdup(mem, tmp);
		free(tmp);
		if (!new_argv[argc])
			return (1);
		argc++;
	}
	cmd->argv = new_argv;
	return (0);
}

static t_redirect	*make_redir(t_shell *sh, t_mem *mem, t_redirect *src)
{
	t_redirect	*dst;
	char		*tmp;

	dst = ms_alloc(mem, sizeof(*dst));
	if (!dst)
		return (NULL);
	dst->type = src->type;
	dst->quoted = src->quoted;
	dst->next = NULL;
	if (src->type == REDIRECT_HEREDOC)
		tmp = strip_quotes(src->target);
	else
		tmp = expand_word(sh, src->target);
	if (!tmp)
		return (NULL);
	dst->target = ms_strdup(mem, tmp);
	free(tmp);
	return (dst);
}

static int	expand_redirects(t_shell *sh, t_mem *mem, t_cmd *cmd)
{
	t_redirect	*src;
	t_redirect	*dst;
	t_redirect	*tail;

	src = cmd->redirects;
	cmd->redirects = NULL;
	tail = NULL;
	while (src)
	{
		dst = make_redir(sh, mem, src);
		if (!dst)
			return (1);
		if (!tail)
			cmd->redirects = dst;
		else
			tail->next = dst;
		tail = dst;
		src = src->next;
	}
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
		if (expand_argv(sh, mem, &pl->cmds[i]) != 0)
			return (1);
		if (expand_redirects(sh, mem, &pl->cmds[i]) != 0)
			return (1);
		i++;
	}
	return (0);
}
