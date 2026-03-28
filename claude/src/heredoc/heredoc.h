/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claude <claude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/03/28 00:00:00 by claude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEREDOC_H
# define HEREDOC_H

# include "../root.h"

int		heredoc_fd(t_shell *sh, char *delim, bool quoted);
void	close_pipeline_heredocs(t_pipeline *pl);
int		gather_heredocs(t_shell *sh, t_pipeline *pl);

#endif
