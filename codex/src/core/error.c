#include "ms.h"

int	ms_perror(char *ctx, char *arg, int code)
{
	if (arg)
		fprintf(stderr, "minishell: %s: %s: %s\n", ctx, arg, strerror(errno));
	else
		fprintf(stderr, "minishell: %s: %s\n", ctx, strerror(errno));
	return (code);
}
