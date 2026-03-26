# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/03/15 15:34:32 by ayusa             #+#    #+#              #
#    Updated: 2026/03/21 11:00:33 by ayusa            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

CC = cc
CFLAGS = -Wall -Wextra -Werror
RLFLAGS     = -lreadline
AR      = ar rcs
RM      = rm -f

SRCDIR      = src
OBJDIR      = obj
LIBFTDIR    = libft
INCDIR      = inc

SRCS = src/signal/signal.c src/signal/signal_heredoc.c \
	src/parser/parser.c  src/parser/parser_helpers.c  src/parser/free_ast.c src/parser/parse_command.c\
	src/main.c  \
	src/status.c \
	src/lexer/lexer.c src/lexer/lexer_helpers.c src/lexer/classify_operator.c\
	src/expander/expand.c  src/expander/expand_string.c  src/expander/expand_utils.c  \
	src/expander/args_expander/expand_args.c  src/expander/args_expander/expand_args_utils.c  \
	src/expander/redirects_expander/expand_redirects.c  \
	src/expander/wildcard/wildcard.c  src/expander/wildcard/wildcard_utils.c  \
	src/executor/exec_line.c src/executor/exec_ast.c  src/executor/pipe.c  \
	src/executor/exec_cmd/exec_cmd.c  src/executor/exec_cmd/redirect.c  \
	src/executor/exec_external/exec_external.c  src/executor/exec_external/get_exec_path.c  \
	src/heredoc/heredoc.c src/heredoc/generate_tmp_filename.c \
	src/env/env_init.c  src/env/env_util.c  src/env/env_access.c  \
	src/builtins/cd.c  src/builtins/echo.c  src/builtins/env.c  src/builtins/exit.c  src/builtins/export.c  src/builtins/export_print.c  src/builtins/pwd.c  src/builtins/unset.c

OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

LIBFT = $(LIBFTDIR)/libft.a
INCLUDES    = -I $(INCDIR) -I $(LIBFTDIR)
HEADERS     = $(INCDIR)/minishell.h

all: $(NAME)

$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) $(RLFLAGS) -o $(NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERS)
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(LIBFT):
	make -C $(LIBFTDIR)

clean:
	make -C $(LIBFTDIR) clean
	rm -rf $(OBJDIR)

fclean: clean
	make -C $(LIBFTDIR) fclean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
