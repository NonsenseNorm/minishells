/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   core_internal.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/03/28 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CORE_INTERNAL_H
# define CORE_INTERNAL_H

# include "core.h"

int		ms_loop(t_shell *sh, char *line);
void	ms_run(t_shell *sh);

#endif
