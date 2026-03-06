NAME    = parser
CC      = cc
CFLAGS  = -Wall -Wextra -Werror

SRCS    = main.c lexer.c lexer_word.c parser.c
OBJS    = $(SRCS:.c=.o)

UNAME := $(shell uname -s)

ifeq ($(UNAME), Darwin)
    RL_DIR = ../readline
    RL_A   = $(RL_DIR)/libreadline.a
    CFLAGS += -I..
    LDLINK = $(RL_A) -lncurses
else
    RL_A   =
    LDLINK = -lreadline
endif

all: $(RL_A) $(NAME)

ifeq ($(UNAME), Darwin)
$(RL_A):
	$(MAKE) -C $(RL_DIR)
endif

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDLINK)

%.o: %.c types.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
