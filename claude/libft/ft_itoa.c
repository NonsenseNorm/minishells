/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claude <claude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/01/01 00:00:00 by claude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	num_len(long n)
{
	size_t	len;

	len = 1;
	if (n < 0)
	{
		len++;
		n = -n;
	}
	while (n >= 10)
	{
		n /= 10;
		len++;
	}
	return (len);
}

char	*ft_itoa(int n)
{
	char	*out;
	long	v;
	size_t	len;

	v = n;
	len = num_len(v);
	out = malloc(len + 1);
	if (!out)
		return (NULL);
	out[len] = '\0';
	if (v < 0)
	{
		out[0] = '-';
		v = -v;
	}
	while (len-- > (size_t)(n < 0))
	{
		out[len] = '0' + (v % 10);
		v /= 10;
	}
	return (out);
}
