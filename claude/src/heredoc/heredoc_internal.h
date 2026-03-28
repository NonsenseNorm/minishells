/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_internal.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claude <claude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/03/28 00:00:00 by claude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEREDOC_INTERNAL_H
# define HEREDOC_INTERNAL_H

# include "heredoc.h"

char	*heredoc_read_line(void);
void	heredoc_write(t_shell *sh, int fd, char *line, bool quoted);
void	heredoc_eof_warning(char *delim);

#endif
