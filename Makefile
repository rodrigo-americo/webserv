NAME = webserver

INCLUDES = -I. -Ilib -Ilib/segregation -Ilib/http -Ilib/text -Ilib/utils -Ilib/schema \
           -Iparser/lexer -Iparser/parser -Iparser/parser/visitors \
           -Iparser/parser/visitors/ParserVisitorValidator

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++17 -g3 $(INCLUDES)

SRC = main.cpp \
      config/ConfigBuilderVisitor.cpp \
      config/GlobalConfig.cpp \
      config/HttpConfig.cpp \
      config/ServerConfig.cpp \
      config/LocationConfig.cpp \
      config/WebServerConfig.cpp

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
	valgrind --track-origins=yes --show-leak-kinds=all --leak-check=full ./$(NAME)

.PHONY: all clean fclean re
