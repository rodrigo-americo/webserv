NAME = lexer

INCLUDES =

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++11 -g3 $(INCLUDES)

SRC = main.cpp

OBJ = $(SRC:%.cpp=%.o)

all: $(NAME)

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
	valgrind --track-origins=yes --show-leak-kinds=all --leak-check=full ./$(NAME)

.PHONY: all clean fclean re
