/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split_c.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/04 14:49:40 by ayusa             #+#    #+#             */
/*   Updated: 2026/03/02 19:07:34 by ayusa            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	count_words(char const *s, char c)
{
	int	count;

	count = 0;
	while (*s)
	{
		while (*s == c)
			s++;
		if (*s)
		{
			count++;
			while (*s && *s != c)
				s++;
		}
	}
	return (count);
}

static void	measure_sizes(char const *s, char c, int *word_sizes)
{
	int	i;
	int	len;

	i = 0;
	while (*s)
	{
		while (*s == c)
			s++;
		if (*s)
		{
			len = 0;
			while (*s && *s != c)
			{
				len++;
				s++;
			}
			word_sizes[i++] = len;
		}
	}
}

static int	fill_words(char const *s, char c, char **result, int *word_sizes)
{
	int	i;
	int	j;

	i = 0;
	while (*s)
	{
		while (*s && *s == c)
			s++;
		if (!(*s))
			break ;
		j = 0;
		result[i] = malloc(word_sizes[i] + 1);
		if (!result[i])
		{
			while (i-- > 0)
				free(result[i]);
			free(result);
			return (0);
		}
		while (*s && *s != c)
			result[i][j++] = *s++;
		result[i++][j] = '\0';
	}
	return (1);
}

static char	**init_split(char const *s, char c, int *wc, int **sizes)
{
	char	**result;

	*wc = count_words(s, c);
	result = malloc(sizeof(char *) * (*wc + 1));
	if (!result)
		return (NULL);
	if (*wc > 0)
	{
		*sizes = malloc(sizeof(int) * (*wc));
		if (!*sizes)
		{
			free(result);
			return (NULL);
		}
		measure_sizes(s, c, *sizes);
	}
	return (result);
}

char	**ft_split_c(char const *s, char c)
{
	char	**result;
	int		word_count;
	int		*word_sizes;

	if (!s)
		return (NULL);
	result = init_split(s, c, &word_count, &word_sizes);
	if (!result)
		return (NULL);
	if (word_count > 0 && !fill_words(s, c, result, word_sizes))
	{
		free(word_sizes);
		return (NULL);
	}
	result[word_count] = NULL;
	if (word_count > 0)
		free(word_sizes);
	return (result);
}

// #include <stdio.h>

// int main(int argc, char* argv[])
// {
// 	char	**words;
// 	if (argc < 3)
// 	{
// 		words = ft_split(" fadvcb  f zv cvsawfs lfd aesca", ' ');
// 	}
// 	else
// 	{
// 		words = ft_split(argv[1], argv[2][0]);
// 	}
// 	int		i = 0;
// 	while (words[i])
// 		printf("%s\n", words[i++]);
// 	return (0);
// }
