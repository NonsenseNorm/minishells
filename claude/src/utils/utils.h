/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::  */
/*   utils.h                                            :+:      :+:    :+:  */
/*                                                    +:+ +:+         +:+    */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+       */
/*                                                +#+#+#+#+#+   +#+          */
/*   Created: 2026/04/10 00:00:00 by stanizak          #+#    #+#            */
/*   Updated: 2026/04/10 00:00:00 by stanizak         ###   ########.fr      */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
# define UTILS_H

# include "../root.h"

int		ft_strcmp(const char *s1, const char *s2);
int		ft_isspace(int c);
long	ft_atol(const char *s, int *ok);
void	ft_free_split(char **arr);

#endif
