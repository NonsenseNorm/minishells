/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 12:05:42 by ayusa             #+#    #+#             */
/*   Updated: 2025/05/08 16:40:40 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t dstsize)
{
	size_t	i;
	size_t	src_len;
	size_t	dst_len;

	dst_len = 0;
	while (dst_len < dstsize && dst[dst_len])
		dst_len++;
	src_len = ft_strlen(src);
	if (dst_len == dstsize)
		return (dstsize + src_len);
	i = 0;
	while (src[i] && (dst_len + i + 1) < dstsize)
	{
		dst[dst_len + i] = src[i];
		i++;
	}
	if (dst_len + i < dstsize)
		dst[dst_len + i] = '\0';
	return (dst_len + src_len);
}

// #include <stdio.h>
// int main(void)
// {
// 	char dst[10] = "abc";
// 	char dst1[10] = "abc";
// 	char src[10] = "def";
// 	int n = 20;

// 	dst[3] = '\0';
// 	dst1[3] = '\0';

// 	printf("%zu\n", ft_strlcat(dst, src, n));
// 	printf("%zu\n", strlcat(dst1, src, n));
// 	printf("%s\n", dst);
// 	printf("%s\n", dst1);
// }
