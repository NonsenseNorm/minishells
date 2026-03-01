/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mem_str.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/02/24 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../core/minishell.h"

void	mem_pop(t_mem *mem, t_mark *mark)
{
	t_block	*b;

	while (mem->top && mem->top != mark->block)
	{
		b = mem->top;
		mem->top = b->prev;
		free(b);
	}
	if (mem->top)
		mem->top->used = mark->used;
}

char	*ms_strndup(t_mem *mem, const char *s, size_t n)
{
	char	*out;

	out = ms_alloc(mem, n + 1);
	if (!out)
		return (NULL);
	ft_memcpy(out, s, n);
	out[n] = 0;
	return (out);
}

char	*ms_strdup(t_mem *mem, const char *s)
{
	return (ms_strndup(mem, s, ft_strlen(s)));
}
