#include "libft.h"

void	*ft_memmove(void *dst, const void *src, size_t len)
{
	size_t				i;
	unsigned char		*d;
	const unsigned char	*s;

	if (!dst && !src)
		return (NULL);
	d = (unsigned char *)dst;
	s = (const unsigned char *)src;
	if (d < s)
		return (ft_memcpy(dst, src, len));
	i = len;
	while (i > 0)
	{
		i--;
		d[i] = s[i];
	}
	return (dst);
}
