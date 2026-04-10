/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strbuf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/01/01 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	sb_init(t_strbuf *sb)
{
	sb->buf = NULL;
	sb->len = 0;
	sb->cap = 0;
}

static int	sb_grow(t_strbuf *sb, size_t need)
{
	size_t	newcap;
	char	*newbuf;

	if (sb->len + need + 1 <= sb->cap)
		return (0);
	newcap = sb->cap;
	if (newcap < 256)
		newcap = 256;
	while (newcap < sb->len + need + 1)
		newcap *= 2;
	newbuf = malloc(newcap);
	if (!newbuf)
		return (-1);
	if (sb->buf)
		ft_memcpy(newbuf, sb->buf, sb->len);
	free(sb->buf);
	sb->buf = newbuf;
	sb->cap = newcap;
	return (0);
}

int	sb_append(t_strbuf *sb, const char *s, size_t n)
{
	if (n == 0)
		return (0);
	if (sb_grow(sb, n) < 0)
		return (-1);
	ft_memcpy(sb->buf + sb->len, s, n);
	sb->len += n;
	sb->buf[sb->len] = '\0';
	return (0);
}

char	*sb_detach(t_strbuf *sb)
{
	char	*ret;

	if (!sb->buf)
		ret = ft_strdup("");
	else
		ret = sb->buf;
	sb->buf = NULL;
	sb->len = 0;
	sb->cap = 0;
	return (ret);
}
