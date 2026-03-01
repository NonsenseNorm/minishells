CC    = cc
AR    = ar
CFLAGS = -O2 -I. -DHAVE_CONFIG_H

SRCS = readline.c vi_mode.c funmap.c keymaps.c parens.c search.c \
       rltty.c complete.c bind.c isearch.c display.c signals.c \
       util.c kill.c undo.c macro.c input.c callback.c terminal.c \
       text.c nls.c misc.c xmalloc.c xfree.c compat.c \
       history.c histexpand.c histfile.c histsearch.c \
       shell.c savestring.c mbutil.c tilde.c colors.c parse-colors.c

OBJS = $(SRCS:.c=.o)

all: libreadline.a

libreadline.a: $(OBJS)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f libreadline.a

re: fclean all

.PHONY: all clean fclean re
