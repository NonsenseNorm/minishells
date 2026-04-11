/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_internal.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/04/11 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXPAND_INTERNAL_H
# define EXPAND_INTERNAL_H

# include "expand.h"

typedef struct s_exp_ctx
{
	char	**nav;
	int		dst;
}	t_exp_ctx;

char	*strip_quotes(const char *s);
size_t	ifs_wc(const char *s);
char	**split_ifs(const char *s);
int		expand_cmd_redirects(t_shell *sh, t_mem *mem, t_cmd *cmd);

#endif
