NAME = ft_traceroute

LIBFT_PATH = ../libft

FLAGS = -Wall -Wextra -Werror -Ofast

INCLUDES_PATH = -Iincludes/ \
				-I$(LIBFT_PATH)/includes/

SRCS_NAME =	main.c \
			ping.c \
			traceroute.c

SRCS_PATH = srcs/

SRCS = $(addprefix $(SRCS_PATH), $(SRCS_NAME))

OBJS_NAME = $(SRCS_NAME:.c=.o)

OBJS_PATH = objs/

OBJS = $(addprefix $(OBJS_PATH), $(OBJS_NAME))

all: $(OBJS_PATH) $(NAME)

$(NAME): $(OBJS)
	make -C $(LIBFT_PATH)
	clang $(FLAGS) -o $(NAME) $(OBJS) $(LIBFT_PATH)/libft.a

$(OBJS_PATH):
	mkdir -p $(OBJS_PATH)

$(OBJS_PATH)%.o: $(SRCS_PATH)%.c
	clang $(FLAGS) -o $@ -c $< $(INCLUDES_PATH)

clean:
	make -C $(LIBFT_PATH) clean
	rm -rf $(OBJS_PATH)

fclean: clean
	make -C $(LIBFT_PATH) fclean
	rm -f $(NAME)

re: fclean all

.PHONY: clean fclean re re-no-lib odir