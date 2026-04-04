/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/29 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/03/29 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*get_next_line(int fd)
{
	t_strbuf	sb;
	char		c;
	ssize_t		rd;

	sb_init(&sb);
	while (1)
	{
		rd = read(fd, &c, 1);
		if (rd <= 0)
		{
			if (rd == 0 && sb.len > 0)
				return (sb_detach(&sb));
			return (free(sb.buf), NULL);
		}
		if (c == '\n')
			break ;
		if (sb_append(&sb, &c, 1) < 0)
			return (NULL);
	}
	return (sb_detach(&sb));
}
