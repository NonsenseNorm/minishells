#include "../core/ms.h"

int	bi_echo(t_cmd *cmd)
{
	int	i;
	int	nl;

	i = 1;
	nl = 1;
	while (cmd->argv[i] && !ft_strcmp(cmd->argv[i], "-n"))
	{
		nl = 0;
		i++;
	}
	while (cmd->argv[i])
	{
		printf("%s", cmd->argv[i]);
		if (cmd->argv[i + 1])
			printf(" ");
		i++;
	}
	if (nl)
		printf("\n");
	return (0);
}

int	bi_pwd(void)
{
	char	cwd[PATH_MAX];

	if (!getcwd(cwd, sizeof(cwd)))
		return (ms_perror("pwd", NULL, 1));
	printf("%s\n", cwd);
	return (0);
}

int	bi_env(t_shell *sh, t_cmd *cmd)
{
	int	i;

	if (cmd->argv[1])
	{
		fprintf(stderr, "minishell: env: too many arguments\n");
		return (1);
	}
	i = 0;
	while (i < sh->env.len)
	{
		if (ft_strchr(sh->env.arr[i], '='))
			printf("%s\n", sh->env.arr[i]);
		i++;
	}
	return (0);
}
