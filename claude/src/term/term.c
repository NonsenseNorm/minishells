/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   term.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/03/31 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "term.h"

void	term_save(t_shell *sh)
{
	if (sh->interactive)
		tcgetattr(STDIN_FILENO, &sh->orig_term);
}

void	term_restore(t_shell *sh)
{
	if (sh->interactive)
		tcsetattr(STDIN_FILENO, TCSADRAIN, &sh->orig_term);
}
