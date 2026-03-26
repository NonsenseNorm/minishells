# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ayusa <ayusa@student.42tokyo.jp>           +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/02 00:02:40 by ayusa             #+#    #+#              #
#    Updated: 2026/03/23 18:36:43 by ayusa            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME    = libft.a

CC      = cc
CFLAGS  = -Wall -Wextra -Werror
AR      = ar rcs
RM      = rm -f

MANDAT_SRCS = \
	ft_atoi.c ft_atol.c ft_itoa.c ft_uitoa.c \
	ft_putchar_fd.c ft_putstr_fd.c ft_putstrn_fd.c \
	ft_putendl_fd.c ft_putnbr_fd.c \
	ft_strcmp_ci.c ft_strlcat.c ft_strlcpy.c ft_strlen.c \
	ft_substr.c ft_strjoin.c ft_strtrim.c ft_strmapi.c \
	ft_striteri.c ft_strcmp.c ft_strncmp.c ft_strnstr.c \
	ft_strchr.c ft_strrchr.c ft_strdup.c \
	ft_bzero.c ft_calloc.c \
	ft_memchr.c ft_memcmp.c ft_memcpy.c ft_memmove.c ft_memset.c \
	ft_split_c.c ft_split.c\
	ft_isalnum.c ft_isalpha.c ft_isascii.c \
	ft_isdigit.c ft_isprint.c \
	ft_tolower.c ft_toupper.c
BONUS_SRCS  = \
	ft_lstnew.c ft_lstadd_back.c ft_lstsize.c ft_lstadd_front.c \
	ft_lstclear.c ft_lstdelone.c ft_lstiter.c ft_lstlast.c ft_lstmap.c

MANDAT_OBJS = $(MANDAT_SRCS:.c=.o)
BONUS_OBJS  = $(BONUS_SRCS:.c=.o)

OBJS = $(MANDAT_OBJS)

ifdef WITH_BONUS
OBJS += $(BONUS_OBJS)
endif

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	$(AR) $(NAME) $(OBJS)

bonus:
	@make WITH_BONUS=1

clean:
	$(RM) $(OBJS) $(BONUS_OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re bonus
