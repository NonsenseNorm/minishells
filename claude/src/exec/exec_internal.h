/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_internal.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claude <claude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/03/28 00:00:00 by claude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXEC_INTERNAL_H
# define EXEC_INTERNAL_H

# include "exec.h"

char	*find_exec_path(t_shell *sh, char *cmd);
void	child_exec(t_shell *sh, t_cmd *cmd);
int		exec_forked_pipeline(t_shell *sh, t_pipeline *pl);
int		apply_redirects(t_shell *sh, t_redirect *r);

#endif
