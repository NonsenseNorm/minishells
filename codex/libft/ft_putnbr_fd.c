#include "libft.h"

void	ft_putnbr_fd(int n, int fd)
{
	long	v;
	char	c;

	v = n;
	if (v < 0)
	{
		write(fd, "-", 1);
		v = -v;
	}
	if (v >= 10)
		ft_putnbr_fd((int)(v / 10), fd);
	c = '0' + (v % 10);
	write(fd, &c, 1);
}
