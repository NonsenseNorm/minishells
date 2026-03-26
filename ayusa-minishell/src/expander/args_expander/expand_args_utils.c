/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_args_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 18:00:00 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/21 21:59:32 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/minishell.h"

// Free the original string and append one character to the end of the string
char	*append_char(char *str, char c)
{
	char	*new_str;
	int		len;
	int		i;

	if (!str)
		return (NULL);
	len = ft_strlen(str);
	new_str = (char *)malloc(sizeof(char) * (len + 2));
	if (!new_str)
	{
		free(str);
		return (NULL);
	}
	i = 0;
	while (i < len)
	{
		new_str[i] = str[i];
		i++;
	}
	new_str[i] = c;
	new_str[i + 1] = '\0';
	free(str);
	return (new_str);
}

// Insert all elements of the `matches` array into the original `args`
char	**insert_matches(char **args, int index,
		char **matches, int match_count)
{
	int		old_len;
	char	**new_args;
	int		k;

	old_len = 0;
	if (!args || !matches)
		return (args);
	while (args[old_len])
		old_len++;
	new_args = malloc(sizeof(char *) * (old_len + match_count));
	if (!new_args)
		return (args);
	k = 0;
	ft_memcpy(new_args + k, args, sizeof(char *) * index);
	k += index;
	ft_memcpy(new_args + k, matches, sizeof(char *) * match_count);
	k += match_count;
	ft_memcpy(new_args + k, args + index + 1,
		sizeof(char *) * (old_len - index - 1));
	k += old_len - index - 1;
	new_args[k] = NULL;
	free(args);
	free(matches);
	return (new_args);
}
