/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_pipeline.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claude <claude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/03/28 00:00:00 by claude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../core/ms.h"

/*
** Expand one argv word into *slot (arena memory).
** POSIX null-word removal: empty expansion of unquoted word → *slot = NULL.
** Returns 0 on success (kept or dropped), 1 on allocation failure.
*/
static int	expand_argv_word(t_shell *sh, t_mem *mem, char *orig, char **slot)
{
	char	*expanded;

	*slot = NULL;
	expanded = expand_word(sh, orig);
	if (!expanded)
		return (1);
	if (*expanded == '\0' && !has_quote(orig))
		return (free(expanded), 0);
	*slot = ms_strdup(mem, expanded);
	free(expanded);
	if (!*slot)
		return (1);
	return (0);
}

/*
** Expand all argv words of one command into arena memory.
** *slot == NULL after expand_argv_word means the word was dropped (null-word
** removal); dst advances only when a word is kept.
*/
static int	expand_cmd_argv(t_shell *sh, t_mem *mem, t_cmd *cmd)
{
	char	**new_argv;
	int		src;
	int		dst;
	int		argc;

	argc = 0;
	while (cmd->argv && cmd->argv[argc])
		argc++;
	new_argv = ms_alloc(mem, sizeof(char *) * (argc + 1));
	if (!new_argv)
		return (1);
	src = 0;
	dst = 0;
	while (cmd->argv && cmd->argv[src])
	{
		if (expand_argv_word(sh, mem, cmd->argv[src], &new_argv[dst]))
			return (1);
		dst += (new_argv[dst] != NULL);
		src++;
	}
	new_argv[dst] = NULL;
	cmd->argv = new_argv;
	return (0);
}

/*
** Deep-copy and expand one redirect into arena memory.
** Heredoc delimiters: strip quotes only (no $-expansion).
** Other targets: full $-expansion and quote removal (expand_word).
*/
static t_redirect	*expand_redir(t_shell *sh, t_mem *mem, t_redirect *src)
{
	t_redirect	*dst;
	char		*tmp;

	dst = ms_alloc(mem, sizeof(*dst));
	if (!dst)
		return (NULL);
	dst->type = src->type;
	dst->quoted = src->quoted;
	dst->fd = src->fd;
	dst->next = NULL;
	if (src->type == REDIRECT_HEREDOC)
		tmp = strip_quotes(src->target);
	else
		tmp = expand_word(sh, src->target);
	if (!tmp)
		return (NULL);
	dst->target = ms_strdup(mem, tmp);
	free(tmp);
	if (!dst->target)
		return (NULL);
	return (dst);
}

/*
** Expand all redirects of one command, rebuilding the linked list
** into arena memory.
*/
static int	expand_cmd_redirects(t_shell *sh, t_mem *mem, t_cmd *cmd)
{
	t_redirect	*src;
	t_redirect	*dst;
	t_redirect	*tail;

	src = cmd->redirects;
	cmd->redirects = NULL;
	tail = NULL;
	while (src)
	{
		dst = expand_redir(sh, mem, src);
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

/*
** Expand all commands in a pipeline.
** The cmds array is deep-copied into exp_mem so that parse_mem can be
** freed immediately after (see ms_loop: parse_mem resets before exec).
*/
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
