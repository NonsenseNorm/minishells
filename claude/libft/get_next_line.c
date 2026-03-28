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

static char	*gnl_join(char *buf, char *add, size_t add_len)
{
	char	*res;
	size_t	buf_len;

	if (!buf)
		buf_len = 0;
	else
		buf_len = ft_strlen(buf);
	res = malloc(buf_len + add_len + 1);
	if (!res)
	{
		free(buf);
		return (NULL);
	}
	if (buf)
		ft_memcpy(res, buf, buf_len);
	ft_memcpy(res + buf_len, add, add_len);
	res[buf_len + add_len] = '\0';
	free(buf);
	return (res);
}

char	*get_next_line(int fd)
{
	char	c;
	char	*line;
	ssize_t	rd;

	line = NULL;
	while (1)
	{
		rd = read(fd, &c, 1);
		if (rd <= 0)
		{
			if (rd == 0 && line)
				return (line);
			free(line);
			return (NULL);
		}
		if (c == '\n')
			break ;
		line = gnl_join(line, &c, 1);
		if (!line)
			return (NULL);
	}
	return (line);
}
