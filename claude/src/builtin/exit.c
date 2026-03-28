/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claude <claude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/01/01 00:00:00 by claude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin_internal.h"

int	bi_exit(t_shell *sh, t_cmd *cmd)
{
	int		ok;
	long	n;

	if (sh->interactive)
		printf("exit\n");
	if (!cmd->argv[1])
		exit(sh->exit_code);
	n = ft_atol(cmd->argv[1], &ok);
	if (!ok)
	{
		fprintf(stderr,
			"minishell: exit: %s: numeric argument required\n",
			cmd->argv[1]);
		exit(2);
	}
	if (cmd->argv[2])
		return (fprintf(stderr,
				"minishell: exit: too many arguments\n"), 1);
	exit((unsigned char)n);
}
