#include "types.h"

/*
** lex_word_end -- *i をワードの末尾まで進める。
**
** ワードが終わる条件 (クォート外):
**   - '\0'
**   - 空白文字
**   - オペレータ文字 | < >
**
** クォート '...' や "..." の内側はすべてワードの一部として読み飛ばす。
**
** 戻り値:
**   0           → 正常終了 (クォートは正しく閉じられた)
**   '\'' or '"' → 閉じられていないクォートの文字
*/
int	lex_word_end(const char *line, int *i)
{
	char	q;

	q = 0;
	while (line[*i])
	{
		if (!q && isspace((unsigned char)line[*i]))
			break ;
		if (!q && strchr("|<>", line[*i]))
			break ;
		if (!q && (line[*i] == '\'' || line[*i] == '"'))
			q = line[*i];
		else if (q && line[*i] == q)
			q = 0;
		(*i)++;
	}
	return ((unsigned char)q);
}
