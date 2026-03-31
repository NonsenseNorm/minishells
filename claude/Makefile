NAME = minishell
CC = cc
READLINE_PREFIX = /usr/local
CFLAGS = -Wall -Wextra -Werror -I$(READLINE_PREFIX)/include
LDFLAGS = -L$(READLINE_PREFIX)/lib -lreadline
LIBFT_DIR = libft
LIBFT_A = $(LIBFT_DIR)/libft.a
SRCS = $(shell find src -name '*.c')
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
