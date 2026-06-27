/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_lexer_token.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/19 21:51:20 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/19 21:51:58 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "test_utils.hpp"
#include "LexerToken.hpp"

#include <sstream>

// ── LexerTokenType / operator<<(ostream&, type) ───────────────────────────────

int test_tokentype_stream_word()
{
	std::ostringstream oss;
	oss << LexerTokenType::WORD;
	return assert(std::string("WORD"), oss.str(), "test_tokentype_stream_word", LINE_DATA());
}

int test_tokentype_stream_first_value()
{
	std::ostringstream oss;
	oss << LexerTokenType::COMMENT; // primeiro do enum (valor 0)
	return assert(std::string("COMMENT"), oss.str(), "test_tokentype_stream_first_value", LINE_DATA());
}

int test_tokentype_stream_last_value()
{
	std::ostringstream oss;
	oss << LexerTokenType::DELIM_ARROW; // último do enum
	return assert(std::string("DELIM_ARROW"), oss.str(), "test_tokentype_stream_last_value", LINE_DATA());
}

int test_tokentype_stream_each_value_matches_name()
{
	// Verifica que TODOS os valores do enum imprimem o nome correspondente,
	// na mesma ordem declarada (serve de teste de regressão para o array
	// LexerTokenTypeStr, que precisa permanecer alinhado com o enum).
	LexerTokenType::type values[] = {
		LexerTokenType::COMMENT, LexerTokenType::LBRACE, LexerTokenType::RBRACE,
		LexerTokenType::SEMICOLON, LexerTokenType::STRING_SINGLE, LexerTokenType::STRING_DOUBLE,
		LexerTokenType::WORD, LexerTokenType::ERROR, LexerTokenType::END,
		LexerTokenType::DELIM_PLUS, LexerTokenType::DELIM_MINUS, LexerTokenType::DELIM_SEMI,
		LexerTokenType::DELIM_ASSIGN, LexerTokenType::DELIM_LBRACE, LexerTokenType::DELIM_RBRACE,
		LexerTokenType::DELIM_ARROW
	};
	const char *names[] = {
		"COMMENT", "LBRACE", "RBRACE", "SEMICOLON", "STRING_SINGLE", "STRING_DOUBLE",
		"WORD", "ERROR", "END", "DELIM_PLUS", "DELIM_MINUS", "DELIM_SEMI",
		"DELIM_ASSIGN", "DELIM_LBRACE", "DELIM_RBRACE", "DELIM_ARROW"
	};
	bool ok = true;
	for (int i = 0; i < 16; i++)
	{
		std::ostringstream oss;
		oss << values[i];
		if (oss.str() != names[i]) { ok = false; break; }
	}
	return assert_true(ok, "test_tokentype_stream_each_value_matches_name", LINE_DATA());
}

// ── BaseToken / LexerToken — construtor com tipo ──────────────────────────────

int test_token_ctor_stores_filename()
{
	LexerToken t("main.txt", 1, 0, "hello", LexerTokenType::WORD);
	return assert(std::string("main.txt"), t.getFileName(), "test_token_ctor_stores_filename", LINE_DATA());
}

int test_token_ctor_stores_line()
{
	LexerToken t("main.txt", 7, 3, "hello", LexerTokenType::WORD);
	return assert_true(t.getLine() == 7, "test_token_ctor_stores_line", LINE_DATA());
}

int test_token_ctor_stores_line_col()
{
	LexerToken t("main.txt", 7, 3, "hello", LexerTokenType::WORD);
	return assert_true(t.getLineColumn() == 3, "test_token_ctor_stores_line_col", LINE_DATA());
}

int test_token_ctor_stores_content()
{
	LexerToken t("main.txt", 1, 0, "hello", LexerTokenType::WORD);
	return assert(std::string("hello"), t.getContent(), "test_token_ctor_stores_content", LINE_DATA());
}

int test_token_ctor_stores_type()
{
	LexerToken t("main.txt", 1, 0, "hello", LexerTokenType::WORD);
	return assert_true(t.getType() == LexerTokenType::WORD, "test_token_ctor_stores_type", LINE_DATA());
}

// ── getLineAddress() ──────────────────────────────────────────────────────────

int test_token_line_address_format()
{
	LexerToken t("main.txt", 3, 12, "x", LexerTokenType::WORD);
	return assert(std::string("main.txt:3:12"), t.getLineAddress(), "test_token_line_address_format", LINE_DATA());
}

int test_token_line_address_with_zero_values()
{
	LexerToken t("a.txt", 0, 0, "x", LexerTokenType::WORD);
	return assert(std::string("a.txt:0:0"), t.getLineAddress(), "test_token_line_address_with_zero_values", LINE_DATA());
}

// ── operator== / operator!= (comparação com Type) ─────────────────────────────

int test_token_eq_type_true()
{
	LexerToken t("f.txt", 1, 0, "x", LexerTokenType::WORD);
	return assert_true(t == LexerTokenType::WORD, "test_token_eq_type_true", LINE_DATA());
}

int test_token_eq_type_false()
{
	LexerToken t("f.txt", 1, 0, "x", LexerTokenType::WORD);
	return assert_true(!(t == LexerTokenType::ERROR), "test_token_eq_type_false", LINE_DATA());
}

int test_token_neq_type_true()
{
	LexerToken t("f.txt", 1, 0, "x", LexerTokenType::WORD);
	return assert_true(t != LexerTokenType::ERROR, "test_token_neq_type_true", LINE_DATA());
}

int test_token_neq_type_false()
{
	LexerToken t("f.txt", 1, 0, "x", LexerTokenType::WORD);
	return assert_true(!(t != LexerTokenType::WORD), "test_token_neq_type_false", LINE_DATA());
}

// ── operator<<(ostream&, LexerToken) — caminho feliz ──────────────────────────

int test_token_stream_basic_no_newline()
{
	LexerToken t("f.txt", 1, 0, "hello", LexerTokenType::WORD);
	std::ostringstream oss;
	oss << t;
	return assert(std::string("<WORD>[hello]"), oss.str(), "test_token_stream_basic_no_newline", LINE_DATA());
}

int test_token_stream_includes_type_name()
{
	LexerToken t("f.txt", 1, 0, "x", LexerTokenType::ERROR);
	std::ostringstream oss;
	oss << t;
	return assert(std::string("<ERROR>[x]"), oss.str(), "test_token_stream_includes_type_name", LINE_DATA());
}

// Feature: quando o conteúdo termina em '\n' (ex.: comentários), o operador
// remove o '\n' do que fica entre os colchetes e reposiciona a quebra de
// linha DEPOIS do fechamento "]"
int test_token_stream_strips_trailing_newline_from_brackets()
{
	LexerToken t("f.txt", 1, 0, "# comment\n", LexerTokenType::COMMENT);
	std::ostringstream oss;
	oss << t;
	return assert(std::string("<COMMENT>[# comment]\n"), oss.str(),
					"test_token_stream_strips_trailing_newline_from_brackets", LINE_DATA());
}

int test_token_stream_content_without_trailing_newline_unaffected()
{
	LexerToken t("f.txt", 1, 0, "word", LexerTokenType::WORD);
	std::ostringstream oss;
	oss << t;
	// sem '\n' final -> sem quebra de linha extra depois do "]"
	return assert(std::string("<WORD>[word]"), oss.str(),
					"test_token_stream_content_without_trailing_newline_unaffected", LINE_DATA());
}

int test_token_stream_empty()
{
	LexerToken t("f.txt", 1, 0, "", LexerTokenType::END);
	std::ostringstream oss;
	oss << t;
	int errors = 0;
	errors += assert(std::string("<END>[]"), oss.str(), "test_token_stream_empty", LINE_DATA());
	return errors;
}

// ── main ──────────────────────────────────────────────────────────────────────

int main(int argc, char **argv)
{
	messages::settup(argc, argv);

	int failures = 0;

	// std::cout << "\n=== LexerTokenType / operator<<(type) ===\n";
	failures += test_tokentype_stream_word();
	failures += test_tokentype_stream_first_value();
	failures += test_tokentype_stream_last_value();
	failures += test_tokentype_stream_each_value_matches_name();

	// std::cout << "\n=== BaseToken / LexerToken — construtor ===\n";
	failures += test_token_ctor_stores_filename();
	failures += test_token_ctor_stores_line();
	failures += test_token_ctor_stores_line_col();
	failures += test_token_ctor_stores_content();
	failures += test_token_ctor_stores_type();

	// std::cout << "\n=== getLineAddress() ===\n";
	failures += test_token_line_address_format();
	failures += test_token_line_address_with_zero_values();

	// std::cout << "\n=== operator== / operator!= ===\n";
	failures += test_token_eq_type_true();
	failures += test_token_eq_type_false();
	failures += test_token_neq_type_true();
	failures += test_token_neq_type_false();

	// std::cout << "\n=== operator<<(ostream&, LexerToken) — caminho feliz ===\n";
	failures += test_token_stream_basic_no_newline();
	failures += test_token_stream_includes_type_name();
	failures += test_token_stream_strips_trailing_newline_from_brackets();
	failures += test_token_stream_content_without_trailing_newline_unaffected();

	// std::cout << "\n=== BUG CRÍTICO: conteúdo vazio causa underflow (UB) ===\n";
	failures += test_token_stream_empty();

	messages::print();

	return failures;
}
