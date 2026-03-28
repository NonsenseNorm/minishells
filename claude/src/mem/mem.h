/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mem.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/03/28 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MEM_H
# define MEM_H

# include "../root.h"

void	mem_init(t_mem *mem);
void	mem_reset(t_mem *mem);
void	*ms_alloc(t_mem *mem, size_t size);
char	*ms_strdup(t_mem *mem, const char *s);
char	*ms_strndup(t_mem *mem, const char *s, size_t n);
void	mem_mark(t_mem *mem, t_mark *mark);
void	mem_pop(t_mem *mem, t_mark *mark);

#endif
