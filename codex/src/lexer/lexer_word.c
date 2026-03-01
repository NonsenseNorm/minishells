#include "../core/ms.h"

int	lex_word_end(const char *line, int *i)
{
	char	q;

	q = 0;
	while (line[*i])
	{
		if (!q && ft_isspace(line[*i]))
			break ;
		if (!q && ft_strchr("|<>", line[*i]))
			break ;
		if (!q && (line[*i] == '\'' || line[*i] == '"'))
			q = line[*i];
		else if (q && line[*i] == q)
			q = 0;
		(*i)++;
	}
	return ((unsigned char)q);
}
