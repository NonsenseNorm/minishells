/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_string.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 16:28:25 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/23 21:30:13 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static int	process_expand_char(char *str, int *i, int *quotes,
				int *has_wildcard);
static char	*handle_dollar(char *res, char *str, int *i, t_env *env_list);
static char	*resolve_dollar_var(char *str, int *i, t_env *env_list);

// Expand $ and quotes
char	*expand_string(char *str, t_env *env_list, int *has_wildcard)
{
	char	*res;
	int		i;
	int		quotes[2];
	int		ret;

	res = ft_strdup("");
	quotes[0] = 0;
	quotes[1] = 0;
	i = 0;
	if (has_wildcard)
		*has_wildcard = 0;
	while (str[i] != '\0')
	{
		ret = process_expand_char(str, &i, quotes, has_wildcard);
		if (ret == -1)
			res = handle_dollar(res, str, &i, env_list);
		else if (ret == 0)
			res = append_char(res, str[i++]);
		if (!res)
			return (NULL);
	}
	return (res);
}

// Processing a single character
static int	process_expand_char(char *str, int *i, int *quotes,
		int *has_wildcard)
{
	if (str[*i] == '\'' && !quotes[1])
	{
		quotes[0] = !quotes[0];
		(*i)++;
		return (1);
	}
	if (str[*i] == '\"' && !quotes[0])
	{
		quotes[1] = !quotes[1];
		(*i)++;
		return (1);
	}
	if (str[*i] == '$' && !quotes[0])
		return (-1);
	if (str[*i] == '*' && !quotes[0] && !quotes[1])
	{
		if (has_wildcard)
			*has_wildcard = 1;
	}
	return (0);
}

// Expand the characters following a single $ -> Join to res
static char	*handle_dollar(char *res, char *str, int *i,
		t_env *env_list)
{
	char	*val;
	char	*new_res;

	val = resolve_dollar_var(str, i, env_list);
	if (!val && str[*i] == '$')
	{
		(*i)++;
		return (append_char(res, '$'));
	}
	if (val)
	{
		new_res = ft_strjoin(res, val);
		free(res);
		return (new_res);
	}
	return (res);
}

// return resolved value
static char	*resolve_dollar_var(char *str, int *i, t_env *env_list)
{
	int		start;
	char	*var_name;
	char	*val;

	(*i)++;
	if (str[*i] == '?')
	{
		(*i)++;
		return (get_env_value(env_list, "?"));
	}
	if (ft_isalnum(str[*i]) || str[*i] == '_')
	{
		start = *i;
		while (str[*i] && (ft_isalnum(str[*i]) || str[*i] == '_'))
			(*i)++;
		var_name = ft_substr(str, start, *i - start);
		val = get_env_value(env_list, var_name);
		free(var_name);
		return (val);
	}
	(*i)--;
	return (NULL);
}
