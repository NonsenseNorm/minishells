#ifndef TYPES_H
# define TYPES_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <ctype.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <errno.h>
# include <readline/readline.h>
# include <readline/history.h>

/* =========================================================
   レキサーが生成するトークン列の型
   ========================================================= */

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

/* =========================================================
   パーサーが生成するパイプライン構造体の型
   ========================================================= */

typedef enum e_redirect_type
{
	REDIRECT_IN,
	REDIRECT_OUT,
	REDIRECT_APPEND,
	REDIRECT_HEREDOC
}	t_redirect_type;

typedef struct s_redirect
{
	t_redirect_type		type;
	char				*target;
	struct s_redirect	*next;
}	t_redirect;

typedef struct s_cmd
{
	char		**argv;
	t_redirect	*redirects;
}	t_cmd;

typedef struct s_pipeline
{
	t_cmd	*cmds;
	int		count;
}	t_pipeline;

/* =========================================================
   関数プロトタイプ
   ========================================================= */

/* lexer.c */
int				lex_word_end(const char *line, int *i);
int				lex_line(const char *line, t_token **out);
void			free_tokens(t_token *lst);

/* parser.c */
int				is_redirect(t_tok_type t);
t_redirect_type	map_redirect(t_tok_type t);
int				parse_pipeline(t_token *tok, t_pipeline *pl);
void			free_pipeline(t_pipeline *pl);

/* exec.c */
char			*find_cmd(const char *argv0);
int				exec_pipeline(t_pipeline *pl);

#endif
