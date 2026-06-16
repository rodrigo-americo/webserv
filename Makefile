NAME = webserver

INCLUDES = $(shell find . -name ".git*" -prune -o -type d -print | sed 's/^/\-I /' | tr "\n" " ")

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++17 -g3 $(INCLUDES)

SRC = main.cpp \
	$(shell find config -name "main.cpp" -prune -o -name "*.cpp" -print | tr "\n" " ")


TEST_SCHEMA_SRC = $(shell find tests/schema -name "*.cpp")

OBJ = $(SRC:%.cpp=%.o)

all: $(NAME)

includes:
	@echo $(INCLUDES)

retry:
	@clear
	@$(MAKE) -s re
	@clear
	@./$(NAME) parser/lexer/example.config

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

%.o: %.cpp
	@$(CC) $(CFLAGS) -c $< -o $@

tests-schema: $(TEST_SCHEMA_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) $(TEST_SCHEMA_SRC) -o tests/schema/test_schema
	./tests/schema/test_schema

tests: tests-schema 

clean:
	@rm -rf $(OBJ)

fclean: clean
	@rm -rf $(NAME)
	@rm -rf tests/schema/test_schema

re: fclean all

valrind: re
	valgrind --track-origins=yes --show-leak-kinds=all --leak-check=full ./$(NAME)

.PHONY: all clean fclean re
