NAME = parser

INCLUDES = $(shell find ../.. -name ".git" -prune -o -type d -print | sed 's/^/\-I /' | tr "\n" " ")

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g3 $(INCLUDES)

SRC = main.cpp

OBJ = $(SRC:%.cpp=%.o)

all: $(NAME)

includes:
	@echo $(INCLUDES)

retry:
	@clear
	@$(MAKE) -s re
	@clear
	@./$(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

%.o: %.cpp
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ)

fclean: clean
	@rm -rf $(NAME)

re: fclean all

valrind: re
	@clear
	valgrind -q --track-origins=yes --show-leak-kinds=all --leak-check=full ./$(NAME) ../lexer/example.config

.PHONY: all clean fclean re
