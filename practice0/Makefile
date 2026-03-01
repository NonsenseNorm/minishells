NAME     = lexer
TEST_BIN = test_lexer
CC       = cc
CFLAGS   = -Wall -Wextra -Werror

SRCS     = main.c lexer.c lexer_word.c
OBJS     = $(SRCS:.c=.o)

TEST_OBJS = test.o lexer.o lexer_word.o

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

test: $(RL_A) $(TEST_BIN)
	./$(TEST_BIN)

ifeq ($(UNAME), Darwin)
$(RL_A):
	$(MAKE) -C $(RL_DIR)
endif

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDLINK)

$(TEST_BIN): $(TEST_OBJS)
	$(CC) $(CFLAGS) $(TEST_OBJS) -o $@ $(LDLINK)

clean:
	rm -f *.o

fclean: clean
	rm -f $(NAME) $(TEST_BIN)

re: fclean all

.PHONY: all test clean fclean re
