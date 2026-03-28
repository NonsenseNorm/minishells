/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo_pwd_env.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claude <claude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/01/01 00:00:00 by claude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../core/ms.h"

static int	is_n_flag(const char *s)
{
	int	i;

	if (!s || s[0] != '-' || s[1] == '\0')
		return (0);
	i = 1;
	while (s[i] == 'n')
		i++;
	return (s[i] == '\0');
}

int	bi_echo(t_cmd *cmd)
{
	int	i;
	int	nl;

	i = 1;
	nl = 1;
	while (cmd->argv[i] && is_n_flag(cmd->argv[i]))
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
