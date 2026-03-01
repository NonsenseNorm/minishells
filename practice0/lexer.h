#ifndef LEXER_H
# define LEXER_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <readline/history.h>
# include <readline/readline.h>

typedef enum e_tok_type
{
	TOK_WORD,
	TOK_PIPE,
	TOK_REDIRECT_IN,
	TOK_REDIRECT_OUT,
	TOK_REDIRECT_APPEND,
	TOK_HEREDOC
}	t_tok_type;

typedef struct s_token
{
	t_tok_type		type;
	char			*value;
	struct s_token	*next;
}	t_token;

t_token	*new_tok(t_tok_type type, char *value);
int		push_tok(t_token **lst, t_token *tok);
int		lex_word_end(const char *line, int *i);
int		lex_line(const char *line, t_token **out);

#endif
