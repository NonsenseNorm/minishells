/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_redir.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/04/11 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expand_internal.h"
#include "../mem/mem.h"
#include "../core/core.h"

static t_redirect	*expand_redir(t_shell *sh, t_mem *mem, t_redirect *src)
{
	t_redirect	*dst;
	char		*tmp;

	dst = ms_alloc(mem, sizeof(*dst));
	if (!dst)
		return (NULL);
	*dst = (t_redirect){src->type, NULL, src->quoted, src->fd, NULL};
	if (src->type == REDIRECT_HEREDOC)
		tmp = strip_quotes(src->target);
	else
		tmp = expand_word(sh, src->target);
	if (!tmp)
		return (NULL);
	if (src->type != REDIRECT_HEREDOC && !tmp[0])
	{
		ms_error(src->target, NULL, "ambiguous redirect", 1);
		return (free(tmp), NULL);
	}
	dst->target = ms_strdup(mem, tmp);
	free(tmp);
	if (!dst->target)
		return (NULL);
	return (dst);
}

int	expand_cmd_redirects(t_shell *sh, t_mem *mem, t_cmd *cmd)
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
