/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/03/28 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin_internal.h"
#include "../core/core.h"

int	bi_env(t_shell *sh, t_cmd *cmd)
{
	int	i;

	if (cmd->argv[1])
	{
		ms_error("env", NULL, "too many arguments", 1);
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
