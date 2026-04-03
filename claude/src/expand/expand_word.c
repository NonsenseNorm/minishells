/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_word.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/01/01 00:00:00 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expand_internal.h"
#include "../env/env.h"

static char	*expand_var(t_shell *sh, const char *s, int *i)
{
	int		st;
	char	*key;
	char	*val;

	if (s[*i + 1] == '?')
	{
		*i += 2;
		return (ft_itoa(sh->exit_code));
	}
	st = *i + 1;
	if (!(ft_isalpha(s[st]) || s[st] == '_'))
		return ((*i)++, ft_strdup("$"));
	while (ft_isalnum(s[*i + 1]) || s[*i + 1] == '_')
		(*i)++;
	key = ft_substr(s, st, *i - st + 1);
	val = env_get(&sh->env, key);
	free(key);
	(*i)++;
	if (!val)
		return (ft_strdup(""));
	return (ft_strdup(val));
}

static char	*append_str(char *a, char *b)
{
	char	*tmp;

	tmp = ft_strjoin(a, b);
	free(a);
	free(b);
	return (tmp);
}

char	*expand_word(t_shell *sh, const char *s)
{
	char	*out;
	char	q;
	int		i;

	out = ft_strdup("");
	q = 0;
	i = 0;
	while (s[i])
	{
		if (!q && (s[i] == '\'' || s[i] == '"'))
			q = s[i++];
		else if (q && s[i] == q)
		{
			q = 0;
			i++;
		}
		else if (s[i] == '$' && q != '\'')
			out = append_str(out, expand_var(sh, s, &i));
		else
			out = append_str(out, ft_substr(s, i++, 1));
		if (!out)
			return (NULL);
	}
	return (out);
}

char	*expand_heredoc_line(t_shell *sh, const char *s)
{
	char	*out;
	int		i;

	out = ft_strdup("");
	i = 0;
	while (s[i])
	{
		if (s[i] == '$')
			out = append_str(out, expand_var(sh, s, &i));
		else
			out = append_str(out, ft_substr(s, i++, 1));
		if (!out)
			return (NULL);
	}
	return (out);
}

char	*strip_quotes(const char *s)
{
	char	*out;
	char	q;
	int		i;
	int		j;

	out = malloc(ft_strlen(s) + 1);
	if (!out)
		return (NULL);
	q = 0;
	i = 0;
	j = 0;
	while (s[i])
	{
		if (!q && (s[i] == '\'' || s[i] == '"'))
			q = s[i++];
		else if (q && s[i] == q)
		{
			q = 0;
			i++;
		}
		else
			out[j++] = s[i++];
	}
	out[j] = 0;
	return (out);
}
