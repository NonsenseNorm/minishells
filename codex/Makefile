NAME = minishell
CC = cc
CFLAGS = -Wall -Wextra -Werror -std=c99
LDFLAGS = -lreadline
LIBFT_DIR = libft
LIBFT_A = $(LIBFT_DIR)/libft.a
SRCS = $(shell find src -name '*.c' -not -path 'src/libft/*')
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
