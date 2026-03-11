#include "types.h"

/* ---- 内部ヘルパー ---- */

static char	*xstrndup(const char *s, size_t n)
{
	char	*p;
	size_t	i;

	p = malloc(n + 1);
	if (!p)
		return (NULL);
	i = 0;
	while (i < n)
	{
		p[i] = s[i];
		i++;
	}
	p[n] = '\0';
	return (p);
}

static t_token	*new_tok(t_tok_type type, char *value)
{
	t_token	*t;

	t = malloc(sizeof(*t));
	if (!t)
		return (NULL);
	t->type = type;
	t->value = value;
	t->next = NULL;
	return (t);
}

static void	push_tok(t_token **lst, t_token *tok)
{
	t_token	*cur;

	if (!*lst)
	{
		*lst = tok;
		return ;
	}
	cur = *lst;
	while (cur->next)
		cur = cur->next;
	cur->next = tok;
}

static void	lex_op(const char *s, int *i, t_token **out)
{
	t_tok_type	type;

	type = TOK_PIPE;
	if (s[*i] == '<' && s[*i + 1] == '<')
		type = TOK_HEREDOC;
	else if (s[*i] == '>' && s[*i + 1] == '>')
		type = TOK_REDIRECT_APPEND;
	else if (s[*i] == '<')
		type = TOK_REDIRECT_IN;
	else if (s[*i] == '>')
		type = TOK_REDIRECT_OUT;
	if (type == TOK_HEREDOC || type == TOK_REDIRECT_APPEND)
		*i += 2;
	else
		*i += 1;
	push_tok(out, new_tok(type, NULL));
}

/* ---- 公開インターフェース ---- */

/*
** lex_line -- 1行をトークン列に変換する。
**
** Returns 0 on success, 1 on syntax error (unclosed quote).
** *out にトークンのリストが書き込まれる。
*/
int	lex_line(const char *line, t_token **out)
{
	int		i;
	int		st;
	int		q;
	char	*word;

	i = 0;
	*out = NULL;
	while (line[i])
	{
		while (line[i] && isspace((unsigned char)line[i]))
			i++;
		if (!line[i])
			break ;
		if (strchr("|<>", line[i]))
			lex_op(line, &i, out);
		else
		{
			st = i;
			q = lex_word_end(line, &i);
			if (q != 0)
			{
				fprintf(stderr,
					"minishell: unexpected EOF while looking for matching `%c'\n",
					q);
				fprintf(stderr,
					"minishell: syntax error: unexpected end of file\n");
				return (1);
			}
			word = xstrndup(line + st, (size_t)(i - st));
			push_tok(out, new_tok(TOK_WORD, word));
		}
	}
	return (0);
}

/*
** free_tokens -- lex_line が生成したトークンリストをすべて解放する。
*/
void	free_tokens(t_token *lst)
{
	t_token	*next;

	while (lst)
	{
		next = lst->next;
		free(lst->value);
		free(lst);
		lst = next;
	}
}
