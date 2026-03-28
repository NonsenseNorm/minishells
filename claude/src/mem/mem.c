/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mem.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/03/26 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "mem.h"

#define MS_MIN_BLOCK 512

static t_block	*new_block(size_t need, t_block *prev)
{
	t_block	*b;
	size_t	sz;

	sz = need;
	if (sz < MS_MIN_BLOCK)
		sz = MS_MIN_BLOCK;
	b = malloc(sizeof(t_block) + sz);
	if (!b)
		return (NULL);
	b->prev = prev;
	b->cap = sz;
	b->used = 0;
	return (b);
}

void	mem_init(t_mem *mem)
{
	mem->top = new_block(MS_MIN_BLOCK, NULL);
}

void	mem_reset(t_mem *mem)
{
	t_block	*cur;
	t_block	*prev;

	cur = mem->top;
	while (cur)
	{
		prev = cur->prev;
		free(cur);
		cur = prev;
	}
	mem->top = NULL;
}

void	*ms_alloc(t_mem *mem, size_t size)
{
	t_block	*b;
	size_t	align;

	align = (size + sizeof(void *) - 1) & ~(sizeof(void *) - 1);
	b = mem->top;
	if (!b || b->used + align > b->cap)
	{
		b = new_block(align, b);
		if (!b)
			return (NULL);
		mem->top = b;
	}
	b->used += align;
	return (b->data + b->used - align);
}
