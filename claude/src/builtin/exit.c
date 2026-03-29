/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/01/01 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin_internal.h"
#include "../core/core.h"

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
		ms_error("exit", cmd->argv[1],
			"numeric argument required", 2);
		exit(2);
	}
	if (cmd->argv[2])
		return (ms_error("exit", NULL,
				"too many arguments", 1));
	exit((unsigned char)n);
}
