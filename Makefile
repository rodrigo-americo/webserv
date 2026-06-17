
SHELL := /bin/bash
# ANSI escape
ESC := \033

# Estilos
BOLD      := $(ESC)[1m
ITALIC    := $(ESC)[3m
UNDERLINE := $(ESC)[4m
STRIKE    := $(ESC)[9m
RESET     := $(ESC)[0m

# Cores
GREEN     := $(ESC)[32m
RED       := $(ESC)[31m

# Atalhos semânticos
SUCCESS   := $(GREEN)
FAIL      := $(RED)

# ****************************************************************************
# ********************************* BEGIN ************************************
# ****************************************************************************


NAME := webserver

INCLUDES = $(shell find . -name ".git*" -prune -o -type d -print | sed 's/^/\-I /' | tr "\n" " ")

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g3 $(INCLUDES)
CFLAGS17 = -Wall -Wextra -Werror -std=c++17 -g3 $(INCLUDES)

TEST_PROGRAM = test
TEST_SRC = test
TEST_MODE =
# TEST_SCHEMA_SRC = $(shell find tests/schema_test -name "*.cpp")

SRC = main.cpp \
	$(shell find config -name "main.cpp" -prune -o -name "*.cpp" -print | tr "\n" " ")



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

# tests-schema: $(TEST_SCHEMA_SRC)
# 	$(CC) $(CFLAGS) $(INCLUDES) $(TEST_SCHEMA_SRC) -o tests/schema_test/test_schema
# 	./tests/schema_test/test_schema

# tests: tests-schema

tests_verbose: TEST_MODE += --verbose
tests_verbose: tests

tests:
	@clear
	@errors=0; \
	setup=$$(find tests/@setup -type f -name "*.cpp" | tr "\n" " "); \
	tests=$$(find tests -type d -name "*_test"); \
	for dir in $$tests; do \
		module=$$(basename $$dir | sed 's/_test$$//'); \
		echo -e "$(BOLD)$(UNDERLINE)module$(RESET): $(BOLD)$$module$(RESET)"; \
		srcs=$$(find $$dir -name "*.cpp"); \
		for test in $$srcs; do \
			name="$$(basename $$test | sed 's/test_//' | sed 's/\.cpp//')"; \
			echo -e "    $(BOLD)$(UNDERLINE)testting$(RESET)... $(BOLD)$$name$(RESET)"; \
			prog="$$dir/$$name-test"; \
			make -s --no-print-directory sub-test TEST_MODE="$(TEST_MODE)" TEST_SRC="$$setup $$test" TEST_PROGRAM="$$prog" 2>&1 | sed 's/^/        /'; \
			status=$${PIPESTATUS[0]}; \
			if [ $$status -ne 0 ]; then \
				errors=$$((errors + $$status)); \
			fi; \
		done; \
	done; \
	if [ $$errors -gt 0 ]; then \
		exit $$errors; \
	fi;

sub-test:
	$(CC) $(CFLAGS17) $(INCLUDES) $(TEST_SRC) -o $(TEST_PROGRAM) && ./$(TEST_PROGRAM) $(TEST_MODE); \
	status=$$?; \
	rm -f $(TEST_PROGRAM); \
	exit $$status

clean:
	@rm -rf $(OBJ)

fclean: clean
	@rm -rf $(NAME)
	@rm -rf tests/schema_test/test_schema

re: fclean all

valrind: re
	valgrind --track-origins=yes --show-leak-kinds=all --leak-check=full ./$(NAME)

.PHONY: tests all clean fclean re
