NAME    = minishell
CC      = cc
CFLAGS  = -Wall -Wextra -Werror

LIBFT_DIR = libft
LIBFT_A   = $(LIBFT_DIR)/libft.a
SRCS      = $(shell find src -name '*.c')
OBJS      = $(SRCS:.c=.o)

# ── Platform detection ────────────────────────────────────────────
UNAME := $(shell uname -s)

ifeq ($(UNAME), Darwin)
    READLINE_DIR    = ../readline
    READLINE_A      = $(READLINE_DIR)/libreadline.a
    READLINE_INC    = -I$(dir $(READLINE_DIR))
    READLINE_LINK   = $(READLINE_A) -lncurses
else
    READLINE_DIR    =
    READLINE_A      =
    READLINE_INC    =
    READLINE_LINK   = -lreadline
endif

CFLAGS  += $(READLINE_INC)

# ── Targets ───────────────────────────────────────────────────────
all: $(READLINE_A) $(NAME)

# Build readline static library from source (macOS only)
$(READLINE_A):
	@if [ ! -f $(READLINE_DIR)/config.h ]; then \
		echo "Configuring readline..."; \
		$(MAKE) -C $(READLINE_DIR) -f /dev/null 2>/dev/null; \
		cd $(READLINE_DIR) && ./configure --quiet; \
	fi
	$(MAKE) -C $(READLINE_DIR) libreadline.a

$(LIBFT_A):
	$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(LIBFT_A) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT_A) -o $(NAME) $(READLINE_LINK)

clean:
	$(MAKE) -C $(LIBFT_DIR) clean
	rm -f $(OBJS)

fclean: clean
	$(MAKE) -C $(LIBFT_DIR) fclean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
