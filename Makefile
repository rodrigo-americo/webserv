
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

VENV = .venv

INCLUDES = $(shell find . -name ".git*" -o -name "$(VENV)*" -prune -o -type d -print | sed 's/^/\-I /' | tr "\n" " ")

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g3 $(INCLUDES)

CFLAGS17 = -Wall -Wextra -Werror -std=c++17 -g3 $(INCLUDES)


TEST_PROGRAM = test
TEST_SRC = test
TEST_LIB = tests/test
TEST_MODE =
TEST_FILTER =
TEST_SETTUP_SRC = $(shell find tests/@setup -type f -name "*.cpp" | tr "\n" " ")
TEST_SETTUP_OBJ = $(TEST_SETTUP_SRC:%.cpp=%.test_o)
# TEST_SCHEMA_SRC = $(shell find tests/schema_test -name "*.cpp")

SRC = main.cpp \
	$(shell find config try01 -name "main.cpp" -prune -o -name "*.cpp" -print | tr "\n" " ")

LIB = lib/lib
LIB_SRC = $(shell find lib -name "main.cpp" -prune -o -type f -name "*.cpp" -print | tr "\n" " ")
LIB_OBJ = $(LIB_SRC:%.cpp=%.lib)

CONFIG_SRC = config/GlobalConfig.cpp config/HttpConfig.cpp config/LocationConfig.cpp config/ServerConfig.cpp config/WebServerConfig.cpp

INTEGRATION_SRC = config/ConfigBuilderVisitor.cpp



OBJ = $(SRC:%.cpp=%.o)

all: $(NAME)

includes:
	@echo $(INCLUDES)

lib_src:
	@echo $(LIB_SRC)

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

venv: $(VENV)

$(VENV):
	@python3 -m venv $(VENV) && \
	source $(VENV)/bin/activate && \
	pip install -r requirements.py_txt && \
	deactivate

tests_coverage: venv
tests_coverage: CFLAGS += --coverage
tests_coverage: CFLAGS17 += --coverage
tests_coverage: TEST_MODE += coverage
tests_coverage: tests

tests_verbose: TEST_MODE += --verbose
tests_verbose: tests

tests: clean $(LIB) $(TEST_LIB)
	@clear
	@errors=0; \
	tests=$$(find tests -type d -name "*_test"); \
	isin() { local list="$$1"; local value="$$2"; if [ -z $$value ]; then echo 0; else echo $$(echo $$list | tr " ," "\n" | perl -0pe 's/\n+/\n/g' |  grep -q $$value && echo 1 || echo 0); fi; }; \
	clear_name() { local name="$$1"; echo $$(basename $$name | sed -E 's/([_]?+test[_]?+)|(\..+)//g'); }; \
	for dir in $$tests; do \
		module=$$(clear_name "$$dir"); \
		srcs=$$(find $$dir -name "*.cpp"); \
		filter_module=1; \
		if [ "$(TEST_FILTER)" ]; then \
			filter_module=$$(isin "$(TEST_FILTER)" $$module); \
		fi; \
		filter_files=""; \
		for file in $$srcs; do \
			if [ $$(isin "$(TEST_FILTER)" $$(clear_name $$file)) = 1 ]; then \
				filter_files="$$filter_files,$$file,"; \
			fi; \
		done; \
		if [ $$filter_module = 0 ] && [ -z "$$filter_files" ]; then continue; fi; \
		echo -e "$(BOLD)$(UNDERLINE)module$(RESET): $(BOLD)$$module$(RESET)"; \
		for file in $$srcs; do \
			name="$$(clear_name $$file)"; \
			if [ $$filter_module = 0 ] && [ $$filter_files ] && [ $$(isin $$filter_files $$file) = 0 ]; then \
				continue; \
			fi; \
			echo -e "    $(BOLD)$(UNDERLINE)testting$(RESET)... $(BOLD)$$name$(RESET)"; \
			prog="$$dir/$$name-test"; \
			make -s --no-print-directory sub-test TEST_MODE="$(TEST_MODE)" CFLAGS="$(CFLAGS)" CFLAGS17="$(CFLAGS17)" TEST_SRC="$$file" TEST_PROGRAM="$$prog" 2>&1 | sed 's/^/        /'; \
			status=$${PIPESTATUS[0]}; \
			if [ $$status -ne 0 ]; then \
				errors=$$((errors + $$status)); \
			fi; \
		done; \
	done; \
	\
	if [ $$(isin "$(TEST_MODE)" coverage) = 1 ]; then \
		source .venv/bin/activate; \
		gcovr -r .;\
		deactivate; \
	fi; \
	if [ $$errors -gt 0 ]; then \
		exit $$errors; \
	fi;

$(LIB): $(LIB_OBJ)
	@ar rcs $@ $^

%.lib: %.cpp
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(TEST_LIB): $(TEST_SETTUP_OBJ)
	@ar rcs $@ $^

%.test_o: %.cpp
	@$(CC) $(CFLAGS17) $(INCLUDES) -c $< -o $@

sub-test:
	extra_src=""; \
	for f in $(TEST_SRC); do \
		case $$f in *integration_test*) extra_src="$(INTEGRATION_SRC)"; break;; esac; \
	done; \
	$(CC) $(CFLAGS17) $(INCLUDES) $(TEST_SRC) $(TEST_LIB) $(LIB_SRC) $(CONFIG_SRC) $$extra_src -o $(TEST_PROGRAM) && ./$(TEST_PROGRAM) $(TEST_MODE); \
	status=$$?; \
	rm -f $(TEST_PROGRAM); \
	exit $$status

clean:
	@rm -rf $(OBJ) $(LIB_OBJ) $(LIB) $(TEST_SETTUP_OBJ) $(TEST_LIB) $(shell find . -type f -name "*.gcda" -o -name "*.gcno" -o -name "*.gcov")

fclean: clean
	@rm -rf $(NAME)
	@rm -rf .venv
	@rm -rf tests/schema_test/test_schema

re: fclean all

valrind: re
	valgrind --track-origins=yes --show-leak-kinds=all --leak-check=full ./$(NAME)

# ****************************************************************************
# ****************************** Testes E2E **********************************
# ****************************************************************************
# Suite de sistema (python + pytest) que sobe o servidor e o ataca via
# sockets crus. Ver tests/e2e/README.md e docs/test_plan.md.

E2E_DIR   = tests/e2e
E2E_MUX  ?= poll
PYTEST    = $(VENV)/bin/python3 -m pytest

# suite rapida (exclui timeouts longos e carga pesada)
e2e: $(NAME) $(VENV)
	@WEBSERV_BIN=$(PWD)/$(NAME) WEBSERV_MUX=$(E2E_MUX) \
		$(PYTEST) $(E2E_DIR) -m "not slow and not stress"

# tudo, inclusive os timeouts reais de 30s
e2e-full: $(NAME) $(VENV)
	@WEBSERV_BIN=$(PWD)/$(NAME) WEBSERV_MUX=$(E2E_MUX) \
		$(PYTEST) $(E2E_DIR) -m "not stress"

# roda a suite nos tres multiplexers
e2e-all-mux: $(NAME)
	@for mux in select poll epoll; do \
		echo "==== MUX=$$mux ===="; \
		WEBSERV_BIN=$(PWD)/$(NAME) WEBSERV_MUX=$$mux \
			$(PYTEST) $(E2E_DIR) -m "not slow and not stress" || exit 1; \
	done

# servidor sob valgrind (leaks de memoria + fds)
leaks: $(NAME)
	@WEBSERV_BIN=$(PWD)/$(NAME) WEBSERV_MUX=$(E2E_MUX) WEBSERV_VALGRIND=1 \
		$(PYTEST) $(E2E_DIR) -m "not slow and not stress" -x

# carga (siege). Requer siege instalado.
stress: $(NAME)
	@WEBSERV_BIN=$(PWD)/$(NAME) WEBSERV_MUX=$(E2E_MUX) \
		$(PYTEST) $(E2E_DIR) -m stress

# roda so os testes cujo nome casa com F=<padrao>, ex: make e2e-focus F=test_k
e2e-focus: $(NAME) $(VENV)
	@WEBSERV_BIN=$(PWD)/$(NAME) WEBSERV_MUX=$(E2E_MUX) \
		$(PYTEST) $(E2E_DIR) -k "$(F)" -m "not slow and not stress"

.PHONY: tests all clean fclean re e2e e2e-full e2e-all-mux leaks stress
