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

char	*expand_word(t_shell *sh, const char *s)
{
	t_strbuf	sb;
	char		q;
	int			i;
	char		*tmp;

	sb_init(&sb);
	q = 0;
	i = 0;
	while (s[i])
	{
		if (!q && (s[i] == '\'' || s[i] == '"'))
			q = s[i++];
		else if (q && s[i] == q && ++i)
			q = 0;
		else if (s[i] == '$' && q != '\'')
		{
			tmp = expand_var(sh, s, &i);
			if (!tmp || sb_append(&sb, tmp, ft_strlen(tmp)) < 0)
				return (free(tmp), free(sb.buf), NULL);
			free(tmp);
		}
		else
			sb_append(&sb, &s[i++], 1);
	}
	return (sb_detach(&sb));
}

char	*expand_heredoc_line(t_shell *sh, const char *s)
{
	t_strbuf	sb;
	int			i;
	char		*tmp;

	sb_init(&sb);
	i = 0;
	while (s[i])
	{
		if (s[i] == '$')
		{
			tmp = expand_var(sh, s, &i);
			if (!tmp || sb_append(&sb, tmp, ft_strlen(tmp)) < 0)
				return (free(tmp), free(sb.buf), NULL);
			free(tmp);
		}
		else
			sb_append(&sb, &s[i++], 1);
	}
	return (sb_detach(&sb));
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
