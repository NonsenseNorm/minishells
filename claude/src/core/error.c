/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claude <claude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/01/01 00:00:00 by claude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ms.h"

void	ms_err(const char *s)
{
	write(2, s, ft_strlen(s));
}

int	ms_perror(char *ctx, char *arg, int code)
{
	ms_err("minishell: ");
	ms_err(ctx);
	if (arg)
	{
		ms_err(": ");
		ms_err(arg);
	}
	ms_err(": ");
	ms_err(strerror(errno));
	ms_err("\n");
	return (code);
}
