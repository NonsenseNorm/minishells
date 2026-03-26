/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 22:09:47 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/18 18:28:55 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

int	builtin_env(t_env *env_list)
{
	char	*tmp_key;

	while (env_list)
	{
		if (env_list->value)
		{
			tmp_key = env_list->key;
			if (tmp_key[0] != '?')
			{
				ft_putstr_fd(tmp_key, STDOUT_FILENO);
				ft_putchar_fd('=', STDOUT_FILENO);
				ft_putendl_fd(env_list->value, STDOUT_FILENO);
			}
		}
		env_list = env_list->next;
	}
	return (0);
}
