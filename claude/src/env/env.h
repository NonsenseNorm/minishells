/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claude <claude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/03/28 00:00:00 by claude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENV_H
# define ENV_H

# include "../root.h"

int		env_init(t_env *env, char **environ);
void	env_free(t_env *env);
char	*env_get(t_env *env, const char *key);
int		env_set(t_env *env, const char *key, const char *val, bool exp);
int		env_unset(t_env *env, const char *key);
int		env_grow(t_env *env);

#endif
