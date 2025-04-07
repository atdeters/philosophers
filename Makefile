NAME		=	filo

COMP		=	cc
CFLAGS		=	-Wall -Wextra -Werror
DBFLAGS		=	-g
LFLAGS		=	-pthread

SRC_FILES	=	main.c helpers.c helpers2.c \
				printer.c check_overflow.c init.c

SRCS		=	$(addprefix src/, $(SRC_FILES))

OFOLDER		=	objs
OPATH		=	objs/
OBJS		=	$(addprefix $(OPATH), $(SRC_FILES:.c=.o))
OBJS_B		=	$(addprefix $(OPATH), $(SRC_FILES_B:.c=.o))

COLOR		=	\033[1;32m
RESET		=	\033[0m

all: $(NAME)

$(OPATH)%.o: src/%.c
	@mkdir -p $(OFOLDER)
	@$(COMP) $(CFLAGS) $(DBFLAGS) $(LFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	@$(COMP) $(CFLAGS) $(DBFLAGS) $(LFLAGS) $(OBJS) -o $(NAME)
	@echo "$(COLOR)make: $(NAME) executed!$(RESET)"

clean:
	@rm -rf $(OBJS)
	@rm -rf $(OFOLDER)
	@echo "$(COLOR)make: clean executed!$(RESET)"

fclean: clean
	@rm -rf 
	@rm -rf $(NAME)
	@echo "$(COLOR)make: fclean executed!$(RESET)"

re: fclean all

tidy: all clean

.PHONY: all clean fclean re tidy