NAME = minishell
CC = cc
READLINE_PREFIX = /usr/local
CFLAGS = -Wall -Wextra -Werror -I$(READLINE_PREFIX)/include
LDFLAGS = -L$(READLINE_PREFIX)/lib -lreadline
LIBFT_DIR = libft
LIBFT_A = $(LIBFT_DIR)/libft.a
SRCS = \
src/builtin/builtin.c \
src/builtin/cd.c \
src/builtin/echo.c \
src/builtin/env.c \
src/builtin/exit.c \
src/builtin/export.c \
src/builtin/pwd.c \
src/builtin/unset.c \
src/core/error.c \
src/core/loop.c \
src/core/main.c \
src/env/env.c \
src/env/env_getset.c \
src/exec/exec.c \
src/exec/path.c \
src/exec/redirect.c \
src/exec/pipeline.c \
src/expand/expand.c \
src/expand/expand_word.c \
src/lexer/lexer.c \
src/lexer/lexer_word.c \
src/mem/mem.c \
src/mem/mem_str.c \
src/parser/parser.c \
src/parser/parser_cmd.c \
src/parser/parser_util.c \
src/signal/signal_exec.c \
src/signal/signal.c \
src/signal/signal_interactive.c \
src/heredoc/heredoc_fd.c \
src/heredoc/heredoc_gather.c \
src/heredoc/heredoc_io.c \
src/term/term.c \

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(LIBFT_A):
	$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(LIBFT_A) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT_A) -o $(NAME) $(LDFLAGS)

clean:
	$(MAKE) -C $(LIBFT_DIR) clean
	rm -f $(OBJS)

fclean: clean
	$(MAKE) -C $(LIBFT_DIR) fclean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
