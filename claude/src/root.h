/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   root.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stanizak <stanizak@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 00:00:00 by stanizak          #+#    #+#             */
/*   Updated: 2026/03/29 09:02:29 by stanizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROOT_H
# define ROOT_H

# include "../libft/libft.h"
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
# include <sys/ioctl.h>
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

typedef struct s_var
{
	char	*key;
	char	*val;
	bool	exported;
}	t_var;

typedef struct s_env
{
	t_var	*vars;
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
	struct termios	orig_term;
	t_mem			*cur_mem;
	char			*cur_input;
	int				exit_code;
	bool			interactive;
}	t_shell;

extern volatile sig_atomic_t	g_sig;

#endif
