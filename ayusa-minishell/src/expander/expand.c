/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 21:28:34 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/23 21:30:51 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static char	*resolve_heredoc_var(char *str, int *i, t_env *env_list);
static char	*expand_heredoc_dollar(char *res, char *str, int *i,
				t_env *env_list);

//  expand args[i] or redir->filename
void	expand_node(t_node *node, t_env *env_list)
{
	if (!node)
		return ;
	if (node->args)
		expand_args(node, env_list);
	expand_redirects(node, env_list);
}

// Heredoc: expand only $VAR/$? — quotes are literal, never stripped
char	*expand_heredoc_line(char *line, t_env *env_list)
{
	char	*res;
	int		i;

	res = ft_strdup("");
	i = 0;
	while (line[i])
	{
		if (line[i] == '$')
			res = expand_heredoc_dollar(res, line, &i, env_list);
		else
			res = append_char(res, line[i++]);
		if (!res)
			return (NULL);
	}
	free(line);
	return (res);
}

static char	*expand_heredoc_dollar(char *res, char *str, int *i,
		t_env *env_list)
{
	char	*val;
	char	*new_res;

	(*i)++;
	if (!ft_isalnum(str[*i]) && str[*i] != '_' && str[*i] != '?')
		return (append_char(res, '$'));
	val = resolve_heredoc_var(str, i, env_list);
	if (!val)
		return (res);
	new_res = ft_strjoin(res, val);
	free(res);
	return (new_res);
}

static char	*resolve_heredoc_var(char *str, int *i, t_env *env_list)
{
	char	*var_name;
	char	*val;
	int		start;

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
	return (NULL);
}
