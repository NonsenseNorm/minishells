/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atol.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claude <claude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/01/01 00:00:00 by claude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <limits.h>

static int	parse_sign(const char **s)
{
	if (**s == '-')
	{
		(*s)++;
		return (-1);
	}
	if (**s == '+')
		(*s)++;
	return (1);
}

static long	overflow_val(int sign)
{
	if (sign == 1)
		return (LONG_MAX);
	return (LONG_MIN);
}

long	ft_atol(const char *s, int *ok)
{
	long	n;
	int		sign;

	*ok = 1;
	while (ft_isspace((unsigned char)*s))
		s++;
	sign = parse_sign(&s);
	if (!ft_isdigit((unsigned char)*s))
		return (*ok = 0, 0);
	n = 0;
	while (ft_isdigit((unsigned char)*s))
	{
		if (n > (LONG_MAX - (*s - '0')) / 10)
			return (*ok = 0, overflow_val(sign));
		n = n * 10 + (*s - '0');
		s++;
	}
	if (*s)
		*ok = 0;
	return (sign * n);
}
