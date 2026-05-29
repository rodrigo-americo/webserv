NAME = schema

INCLUDES = $(shell find .. -type d | sed 's/^/-I /' | tr "\n" " ")

CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98 $(INCLUDES)

SRC = main.cpp

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

teste:
	echo $(INCLUDES)

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re