/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_gather.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claude <claude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/03/27 00:00:00 by claude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "heredoc.h"

void	close_pipeline_heredocs(t_pipeline *pl)
{
	t_redirect	*r;
	int			i;

	i = 0;
	while (i < pl->count)
	{
		r = pl->cmds[i].redirects;
		while (r)
		{
			if (r->type == REDIRECT_HEREDOC && r->fd >= 0)
			{
				close(r->fd);
				r->fd = -1;
			}
			r = r->next;
		}
		i++;
	}
}

int	gather_heredocs(t_shell *sh, t_pipeline *pl)
{
	t_redirect	*r;
	int			i;

	i = 0;
	while (i < pl->count)
	{
		r = pl->cmds[i].redirects;
		while (r)
		{
			if (r->type == REDIRECT_HEREDOC)
			{
				r->fd = heredoc_fd(sh, r->target, r->quoted);
				if (r->fd < 0)
				{
					close_pipeline_heredocs(pl);
					return (-1);
				}
			}
			r = r->next;
		}
		i++;
	}
	return (0);
}
