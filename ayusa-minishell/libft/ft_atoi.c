/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 14:42:51 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/21 16:20:38 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	check_overflow(int sign, long current, char next)
{
	int	digit;

	digit = next - '0';
	if (sign == 1)
	{
		if (current > LONG_MAX / 10
			|| (current == LONG_MAX / 10 && digit > LONG_MAX % 10))
			return (1);
	}
	if (sign == -1)
	{
		if (current > -(LONG_MIN / 10)
			|| (current == -(LONG_MIN / 10) && digit > -(LONG_MIN % 10)))
			return (-1);
	}
	return (0);
}

static int	ft_atoi_sign(const char **str)
{
	while (**str == 32 || (**str >= 9 && **str <= 13))
		(*str)++;
	if (**str == '-')
	{
		(*str)++;
		return (-1);
	}
	if (**str == '+')
		(*str)++;
	return (1);
}

int	ft_atoi(const char *str)
{
	int					sign;
	unsigned long int	result;

	if (!str)
		return (0);
	result = 0;
	sign = ft_atoi_sign(&str);
	while (*str >= '0' && *str <= '9')
	{
		if (check_overflow(sign, result, *str) == 1)
			return ((int)LONG_MAX);
		if (check_overflow(sign, result, *str) == -1)
			return ((int)LONG_MIN);
		result = result * 10 + (long)(*str - '0');
		str++;
	}
	return ((int)(result * sign));
}

// #include <stdlib.h>
// #include <stdio.h>

// int main(void)
// {
// 	char a[15] = "+  214748";
// 	char b[11] = "+2147";

// 	printf("%d\n", atoi(a));
// 	printf("%d\n\n", ft_atoi(a));

// 	printf("%d\n", atoi(b));
// 	printf("%d\n\n", ft_atoi(b));
// }
