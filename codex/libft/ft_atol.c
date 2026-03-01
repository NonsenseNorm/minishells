#include "libft.h"

long	ft_atol(const char *s, int *ok)
{
	long	n;
	long	sign;

	n = 0;
	sign = 1;
	*ok = 0;
	while (ft_isspace(*s))
		s++;
	if (*s == '+' || *s == '-')
	{
		if (*s == '-')
			sign = -1;
		s++;
	}
	if (!ft_isdigit(*s))
		return (0);
	while (ft_isdigit(*s))
	{
		n = n * 10 + (*s - '0');
		s++;
	}
	while (ft_isspace(*s))
		s++;
	if (*s)
		return (0);
	*ok = 1;
	return (n * sign);
}
