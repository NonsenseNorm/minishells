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
#include "../term/term.h"

static void	clean_exit(t_shell *sh, int code)
{
	term_restore(sh);
	shell_cleanup(sh);
	exit(code);
}

int	bi_exit(t_shell *sh, t_cmd *cmd)
{
	int		ok;
	long	n;

	if (sh->interactive)
		printf("exit\n");
	if (!cmd->argv[1])
		clean_exit(sh, sh->exit_code);
	n = ft_atol(cmd->argv[1], &ok);
	if (!ok)
	{
		ms_error("exit", cmd->argv[1],
			"numeric argument required", 2);
		clean_exit(sh, 2);
	}
	if (cmd->argv[2])
		return (ms_error("exit", NULL,
				"too many arguments", 1));
	clean_exit(sh, (unsigned char)n);
	return (0);
}
