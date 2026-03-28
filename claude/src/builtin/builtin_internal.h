/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_internal.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/03/28 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILTIN_INTERNAL_H
# define BUILTIN_INTERNAL_H

# include "builtin.h"

int	valid_ident(const char *s);
int	bi_echo(t_cmd *cmd);
int	bi_cd(t_shell *sh, t_cmd *cmd);
int	bi_pwd(void);
int	bi_export(t_shell *sh, t_cmd *cmd);
int	bi_unset(t_shell *sh, t_cmd *cmd);
int	bi_env(t_shell *sh, t_cmd *cmd);
int	bi_exit(t_shell *sh, t_cmd *cmd);

#endif
