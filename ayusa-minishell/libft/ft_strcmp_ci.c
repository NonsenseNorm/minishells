/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcmp_ci.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 18:35:14 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/23 18:36:13 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strcmp_ci(const char *a, const char *b)
{
	unsigned char	ca;
	unsigned char	cb;

	while (*a && *b)
	{
		ca = (unsigned char)*a;
		cb = (unsigned char)*b;
		if (ca >= 'A' && ca <= 'Z')
			ca += 32;
		if (cb >= 'A' && cb <= 'Z')
			cb += 32;
		if (ca != cb)
			return (ca - cb);
		a++;
		b++;
	}
	ca = (unsigned char)*a;
	cb = (unsigned char)*b;
	if (ca >= 'A' && ca <= 'Z')
		ca += 32;
	if (cb >= 'A' && cb <= 'Z')
		cb += 32;
	return (ca - cb);
}
