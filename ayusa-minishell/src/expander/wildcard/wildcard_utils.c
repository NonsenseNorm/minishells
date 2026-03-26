/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 16:58:05 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/23 18:37:47 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/minishell.h"

static int	match_step(char **pat, char **str, char **star, char **match);
static void	swap_if_needed(char **array, int j);

// Check if the pattern matches the filename
int	match_pattern(char *pattern, char *str)
{
	char	*star_idx;
	char	*match;

	star_idx = NULL;
	match = NULL;
	while (*str)
	{
		if (match_step(&pattern, &str, &star_idx, &match) == -1)
			return (0);
	}
	while (*pattern == '*')
		pattern++;
	if (*pattern == '\0')
		return (1);
	return (0);
}

static int	match_step(char **pat, char **str, char **star, char **match)
{
	if (**pat == **str)
	{
		(*pat)++;
		(*str)++;
	}
	else if (**pat == '*')
	{
		*star = *pat;
		*match = *str;
		(*pat)++;
	}
	else if (*star != NULL)
	{
		*pat = *star + 1;
		(*match)++;
		*str = *match;
	}
	else
		return (-1);
	return (0);
}

void	sort_str_array(char **array, int count)
{
	int	i;
	int	j;

	if (!array || count < 2)
		return ;
	i = 0;
	while (i < count - 1)
	{
		j = 0;
		while (j < count - i - 1)
		{
			swap_if_needed(array, j);
			j++;
		}
		i++;
	}
}

static void	swap_if_needed(char **array, int j)
{
	char	*tmp;

	if (ft_strcmp_ci(array[j], array[j + 1]) > 0)
	{
		tmp = array[j];
		array[j] = array[j + 1];
		array[j + 1] = tmp;
	}
}
