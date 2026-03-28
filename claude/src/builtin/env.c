/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claude <claude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/03/28 00:00:00 by claude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtin_internal.h"

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
