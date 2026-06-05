NAME = validation

INCLUDES = -I ../text -I ../utils -I ../structure -I ../segregation

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++11 -g3 $(INCLUDES)

SRC = main.cpp

OBJ = $(SRC:%.cpp=%.o)

all: $(NAME)

dijkstra: re
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

retry: re
	@clear
	@./$(NAME)

valrind: re
	valgrind --track-origins=yes --show-leak-kinds=all --leak-check=full ./$(NAME)

.PHONY: all clean fclean re
