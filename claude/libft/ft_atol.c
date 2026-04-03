/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atol.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/01/01 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <limits.h>

static int	skip_ws_sign(const char **s)
{
	while (ft_isspace((unsigned char)**s))
		(*s)++;
	if (**s == '-')
		return ((*s)++, -1);
	if (**s == '+')
		(*s)++;
	return (1);
}

static long	parse_digits(const char **s, int sign, int *ok)
{
	unsigned long	limit;
	unsigned long	n;

	if (sign == 1)
		limit = LONG_MAX;
	else
		limit = (unsigned long)LONG_MAX + 1;
	n = 0;
	while (ft_isdigit((unsigned char)**s))
	{
		if (n > (limit - (**s - '0')) / 10)
		{
			*ok = 0;
			if (sign == 1)
				return (LONG_MAX);
			return (LONG_MIN);
		}
		n = n * 10 + (**s - '0');
		(*s)++;
	}
	if (sign == -1)
		return ((long)(~n + 1));
	return ((long)n);
}

long	ft_atol(const char *s, int *ok)
{
	long	n;
	int		sign;

	*ok = 1;
	sign = skip_ws_sign(&s);
	if (!ft_isdigit((unsigned char)*s))
		return (*ok = 0, 0);
	n = parse_digits(&s, sign, ok);
	while (ft_isspace((unsigned char)*s))
		s++;
	if (*s)
		*ok = 0;
	return (n);
}
