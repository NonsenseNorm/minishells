/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ms.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claude <claude@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by claude            #+#    #+#             */
/*   Updated: 2026/01/01 00:00:00 by claude           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MS_H
# define MS_H

# include "../../libft/libft.h"
# include <errno.h>
# include <fcntl.h>
# include <limits.h>
# include <stdio.h>
# include <stdlib.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdbool.h>
# include <stddef.h>
# include <stdint.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <termios.h>
# include <unistd.h>

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
	bool				quoted;
	int					fd;
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

typedef struct s_env
{
	char	**arr;
	int		len;
	int		cap;
}	t_env;

typedef struct s_block
{
	struct s_block	*prev;
	size_t			cap;
	size_t			used;
	char			data[];
}	t_block;

typedef struct s_mark
{
	t_block	*block;
	size_t	used;
}	t_mark;

typedef struct s_mem
{
	t_block	*top;
}	t_mem;

typedef struct s_shell
{
	t_env			env;
	int				exit_code;
	bool			interactive;
	bool			term_saved;
	struct termios	term_orig;
}	t_shell;

extern volatile sig_atomic_t	g_sig;

void	mem_init(t_mem *mem);
void	mem_reset(t_mem *mem);
void	*ms_alloc(t_mem *mem, size_t size);
char	*ms_strdup(t_mem *mem, const char *s);
char	*ms_strndup(t_mem *mem, const char *s, size_t n);
void	mem_mark(t_mem *mem, t_mark *mark);
void	mem_pop(t_mem *mem, t_mark *mark);

int		env_init(t_env *env, char **environ);
void	env_free(t_env *env);
char	*env_get(t_env *env, const char *key);
int		env_set(t_env *env, const char *key, const char *val, bool exp);
int		env_unset(t_env *env, const char *key);
int		env_grow(t_env *env);

void	sig_set_interactive(void);
void	sig_set_exec_parent(void);
void	sig_set_exec_child(void);
void	sig_set_heredoc(void);

int		lex_word_end(const char *line, int *i);
int		lex_line(t_shell *sh, t_mem *mem, const char *line, t_token **out);
int		has_quote(const char *s);
int		is_redirect(t_tok_type t);
int		syntax_err(t_shell *sh, t_token *tok);
int		parse_cmds(t_shell *sh, t_mem *mem, t_token *tok, t_pipeline *pl);
int		parse_pipeline(t_shell *sh, t_mem *mem, t_token *tok, t_pipeline *pl);
char	*expand_word(t_shell *sh, const char *s);
char	*strip_quotes(const char *s);
int		expand_pipeline(t_shell *sh, t_mem *mem, t_pipeline *pl);

char	*find_exec_path(t_shell *sh, char *cmd);
int		heredoc_fd(t_shell *sh, char *delim, bool quoted);
void	close_pipeline_heredocs(t_pipeline *pl);
int		gather_heredocs(t_shell *sh, t_pipeline *pl);
int		apply_redirects(t_shell *sh, t_redirect *r);
int		exec_forked_pipeline(t_shell *sh, t_pipeline *pl);
void	child_exec(t_shell *sh, t_cmd *cmd);
int		exec_pipeline(t_shell *sh, t_pipeline *pl);
int		is_parent_builtin(t_cmd *cmd);
int		run_builtin(t_shell *sh, t_cmd *cmd);

int		bi_echo(t_cmd *cmd);
int		bi_cd(t_shell *sh, t_cmd *cmd);
int		bi_pwd(void);
int		bi_export(t_shell *sh, t_cmd *cmd);
int		bi_unset(t_shell *sh, t_cmd *cmd);
int		bi_env(t_shell *sh, t_cmd *cmd);
int		bi_exit(t_shell *sh, t_cmd *cmd);

void	ms_err(const char *s);
int		ms_perror(char *ctx, char *arg, int code);
int		ms_loop(t_shell *sh, char *line);
void	ms_run(t_shell *sh);
void	ms_term_disable_echoctl(t_shell *sh);

#endif
