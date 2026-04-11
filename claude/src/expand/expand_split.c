/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_split.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/11 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/04/11 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expand_internal.h"

static int	is_ifs(char c)
{
	return (c == ' ' || c == '\t' || c == '\n');
}

size_t	ifs_wc(const char *s)
{
	size_t	n;

	n = 0;
	while (*s)
	{
		while (*s && is_ifs(*s))
			s++;
		if (*s)
			n++;
		while (*s && !is_ifs(*s))
			s++;
	}
	return (n);
}

char	**split_ifs(const char *s)
{
	char	**arr;
	size_t	i;
	size_t	st;
	size_t	k;

	arr = ft_calloc(ifs_wc(s) + 1, sizeof(char *));
	if (!arr)
		return (NULL);
	i = 0;
	k = 0;
	while (s[i])
	{
		while (s[i] && is_ifs(s[i]))
			i++;
		st = i;
		while (s[i] && !is_ifs(s[i]))
			i++;
		if (i > st)
			arr[k++] = ft_substr(s, (unsigned int)st, i - st);
		if (i > st && !arr[k - 1])
			return (ft_free_split(arr), NULL);
	}
	return (arr);
}
