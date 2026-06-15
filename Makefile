NAME = webserver

INCLUDES = 

TEST_INCLUDES = -I lib/schema

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g3 $(INCLUDES)

SRC = main.cpp

TEST_SCHEMA_SRC = tests/schema/test_string.cpp

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

tests-schema: $(TEST_SCHEMA_SRC)
	$(CC) $(CFLAGS) -I lib/schema -I lib/utils -I tests $(TEST_SCHEMA_SRC) -o tests/schema/test_schema
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
