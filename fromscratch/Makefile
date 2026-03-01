NAME      = minishell
CC        = cc
CFLAGS    = -Wall -Wextra -Werror

LIBFT_DIR = libft
LIBFT_A   = $(LIBFT_DIR)/libft.a

SRCS      = $(shell find src -name '*.c')
OBJS      = $(SRCS:.c=.o)

# ── Platform detection ────────────────────────────────────────────
UNAME := $(shell uname -s)

ifeq ($(UNAME), Darwin)
    RL_DIR  = ../readline
    RL_A    = $(RL_DIR)/libreadline.a
    CFLAGS += -I..
    LDLINK  = $(RL_A) -lncurses
else
    RL_DIR  =
    RL_A    =
    LDLINK  = -lreadline
endif

# ── Targets ───────────────────────────────────────────────────────
all: $(RL_A) $(LIBFT_A) $(NAME)

ifeq ($(UNAME), Darwin)
$(RL_A):
	$(MAKE) -C $(RL_DIR)
endif

$(LIBFT_A):
	$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(RL_A) $(LIBFT_A) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT_A) -o $@ $(LDLINK)

clean:
	$(MAKE) -C $(LIBFT_DIR) clean
	rm -f $(OBJS)

fclean: clean
	$(MAKE) -C $(LIBFT_DIR) fclean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
