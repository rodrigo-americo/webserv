/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_builder.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/19 22:47:38 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/19 22:47:40 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "test_utils.hpp"
#include "LexerBuilder.hpp"

#include <fstream>

// ── helpers ──────────────────────────────────────────────────────────────────

static void write_file(const std::string &path, const std::string &content)
{
	std::ofstream f(path.c_str());
	f << content;
}

// ── withFile() ────────────────────────────────────────────────────────────────

int test_withFile_loads_target_file()
{
	write_file("/tmp/lb_t1.txt", "hello world");
	LexerIterator it = LexerBuilder().withFile("/tmp/lb_t1.txt").build();
	bool ok = it.size() >= 1 && (*it).getContent() == "hello";
	return assert_true(ok, "test_withFile_loads_target_file");
}

int test_withFile_returns_self_for_chaining()
{
	LexerBuilder b;
	LexerBuilder &ref = b.withFile("/tmp/lb_t2.txt");
	return assert_true(&ref == &b, "test_withFile_returns_self_for_chaining");
}

int test_missing_file_propagates_error_to_iterator()
{
	std::remove("/tmp/lb_missing.txt");
	LexerIterator it = LexerBuilder().withFile("/tmp/lb_missing.txt").build();
	bool ok = !it.error().empty()
		&& it.error().find("Cannot open file") != std::string::npos;
	return assert_true(ok, "test_missing_file_propagates_error_to_iterator");
}

// ── withDefaultTokens() ───────────────────────────────────────────────────────

int test_withDefaultTokens_recognizes_braces()
{
	write_file("/tmp/lb_t3.txt", "{ x }");
	LexerIterator it = LexerBuilder().withFile("/tmp/lb_t3.txt").withDefaultTokens().build();
	bool ok = it.size() >= 3
		&& (*it).getType() == LexerTokenType::LBRACE;
	++it;
	ok = ok && (*it).getContent() == "x";
	++it;
	ok = ok && (*it).getType() == LexerTokenType::RBRACE;
	return assert_true(ok, "test_withDefaultTokens_recognizes_braces");
}

int test_withDefaultTokens_recognizes_semicolon()
{
	write_file("/tmp/lb_t4.txt", "a;b");
	LexerIterator it = LexerBuilder().withFile("/tmp/lb_t4.txt").withDefaultTokens().build();
	++it; // pula 'a'
	bool ok = (*it).getType() == LexerTokenType::SEMICOLON && (*it).getContent() == ";";
	return assert_true(ok, "test_withDefaultTokens_recognizes_semicolon");
}

int test_withDefaultTokens_recognizes_double_quote()
{
	write_file("/tmp/lb_t5.txt", "\"hi\"");
	LexerIterator it = LexerBuilder().withFile("/tmp/lb_t5.txt").withDefaultTokens().build();
	bool ok = (*it).getType() == LexerTokenType::STRING_DOUBLE && (*it).getContent() == "hi";
	return assert_true(ok, "test_withDefaultTokens_recognizes_double_quote");
}

int test_withDefaultTokens_recognizes_single_quote()
{
	write_file("/tmp/lb_t6.txt", "'hi'");
	LexerIterator it = LexerBuilder().withFile("/tmp/lb_t6.txt").withDefaultTokens().build();
	bool ok = (*it).getType() == LexerTokenType::STRING_SINGLE && (*it).getContent() == "hi";
	return assert_true(ok, "test_withDefaultTokens_recognizes_single_quote");
}

int test_withDefaultTokens_recognizes_hash_comment()
{
	write_file("/tmp/lb_t7.txt", "a # comment");
	LexerIterator it = LexerBuilder().withFile("/tmp/lb_t7.txt").withDefaultTokens().build();
	++it; // pula 'a'
	bool ok = (*it).getType() == LexerTokenType::COMMENT;
	return assert_true(ok, "test_withDefaultTokens_recognizes_hash_comment");
}

int test_withDefaultTokens_returns_self_for_chaining()
{
	LexerBuilder b;
	LexerBuilder &ref = b.withDefaultTokens();
	return assert_true(&ref == &b, "test_withDefaultTokens_returns_self_for_chaining");
}

// ── withComment() ─────────────────────────────────────────────────────────────

int test_withComment_custom_prefix()
{
	write_file("/tmp/lb_t8.txt", "x // c-style comment");
	LexerIterator it = LexerBuilder().withFile("/tmp/lb_t8.txt").withComment("//").build();
	++it; // pula 'x'
	bool ok = (*it).getType() == LexerTokenType::COMMENT;
	return assert_true(ok, "test_withComment_custom_prefix");
}

int test_withComment_returns_self_for_chaining()
{
	LexerBuilder b;
	LexerBuilder &ref = b.withComment("//");
	return assert_true(&ref == &b, "test_withComment_returns_self_for_chaining");
}

// ── withQuote() ───────────────────────────────────────────────────────────────

int test_withQuote_custom_delimiter()
{
	write_file("/tmp/lb_t9.txt", "`raw`");
	LexerIterator it = LexerBuilder()
		.withFile("/tmp/lb_t9.txt")
		.withQuote("`", LexerTokenType::STRING_SINGLE)
		.build();
	bool ok = (*it).getType() == LexerTokenType::STRING_SINGLE && (*it).getContent() == "raw";
	return assert_true(ok, "test_withQuote_custom_delimiter");
}

int test_withQuote_returns_self_for_chaining()
{
	LexerBuilder b;
	LexerBuilder &ref = b.withQuote("`", LexerTokenType::STRING_SINGLE);
	return assert_true(&ref == &b, "test_withQuote_returns_self_for_chaining");
}

int test_withQuote_multiple_distinct_delimiters()
{
	write_file("/tmp/lb_t10.txt", "`a` ~b~");
	LexerIterator it = LexerBuilder()
		.withFile("/tmp/lb_t10.txt")
		.withQuote("`", LexerTokenType::STRING_SINGLE)
		.withQuote("~", LexerTokenType::STRING_DOUBLE)
		.build();
	bool ok = (*it).getType() == LexerTokenType::STRING_SINGLE && (*it).getContent() == "a";
	++it;
	ok = ok && (*it).getType() == LexerTokenType::STRING_DOUBLE && (*it).getContent() == "b";
	return assert_true(ok, "test_withQuote_multiple_distinct_delimiters");
}

// ── withKeyword() ─────────────────────────────────────────────────────────────

int test_withKeyword_custom_delimiter()
{
	write_file("/tmp/lb_t11.txt", "a=>b");
	LexerIterator it = LexerBuilder()
		.withFile("/tmp/lb_t11.txt")
		.withKeyword("=>", LexerTokenType::DELIM_ARROW)
		.build();
	++it; // pula 'a'
	bool ok = (*it).getType() == LexerTokenType::DELIM_ARROW && (*it).getContent() == "=>";
	return assert_true(ok, "test_withKeyword_custom_delimiter");
}

int test_withKeyword_returns_self_for_chaining()
{
	LexerBuilder b;
	LexerBuilder &ref = b.withKeyword("=>", LexerTokenType::DELIM_ARROW);
	return assert_true(&ref == &b, "test_withKeyword_returns_self_for_chaining");
}

int test_withKeyword_multiple_distinct()
{
	write_file("/tmp/lb_t12.txt", "a+b-c");
	LexerIterator it = LexerBuilder()
		.withFile("/tmp/lb_t12.txt")
		.withKeyword("+", LexerTokenType::DELIM_PLUS)
		.withKeyword("-", LexerTokenType::DELIM_MINUS)
		.build();
	++it; // pula 'a'
	bool ok = (*it).getType() == LexerTokenType::DELIM_PLUS;
	it += 2; // pula 'b', chega no '-'
	ok = ok && (*it).getType() == LexerTokenType::DELIM_MINUS;
	return assert_true(ok, "test_withKeyword_multiple_distinct");
}

// ── method chaining completo ──────────────────────────────────────────────────

int test_full_chain_combines_all_configs()
{
	write_file("/tmp/lb_t13.txt", "key => \"value\"; # done");
	LexerIterator it = LexerBuilder()
		.withFile("/tmp/lb_t13.txt")
		.withKeyword("=>", LexerTokenType::DELIM_ARROW)
		.withKeyword(";", LexerTokenType::SEMICOLON)
		.withQuote("\"", LexerTokenType::STRING_DOUBLE)
		.withComment("#")
		.build();

	bool ok = (*it).getContent() == "key"; ++it;
	ok = ok && (*it).getType() == LexerTokenType::DELIM_ARROW; ++it;
	ok = ok && (*it).getType() == LexerTokenType::STRING_DOUBLE && (*it).getContent() == "value"; ++it;
	ok = ok && (*it).getType() == LexerTokenType::SEMICOLON; ++it;
	ok = ok && (*it).getType() == LexerTokenType::COMMENT;
	return assert_true(ok, "test_full_chain_combines_all_configs");
}

// Feature: a ordem das chamadas no chaining não afeta o resultado, pois cada
// método apenas configura o Lexer interno sem depender de uma sequência —
// chamar withDefaultTokens() antes de withFile() produz o mesmo resultado
// que chamar depois.
int test_chain_order_does_not_matter()
{
	write_file("/tmp/lb_t14.txt", "z");
	LexerIterator it_first = LexerBuilder()
		.withFile("/tmp/lb_t14.txt")
		.withDefaultTokens()
		.build();

	write_file("/tmp/lb_t14b.txt", "z");
	LexerIterator it_second = LexerBuilder()
		.withDefaultTokens()
		.withFile("/tmp/lb_t14b.txt")
		.build();

	bool ok = it_first.error().empty() && it_second.error().empty()
		&& (*it_first).getContent() == (*it_second).getContent();
	return assert_true(ok, "test_chain_order_does_not_matter");
}

// ── build() ───────────────────────────────────────────────────────────────────

int test_build_returns_iterator_with_correct_size()
{
	write_file("/tmp/lb_t15.txt", "a b c");
	LexerIterator it = LexerBuilder().withFile("/tmp/lb_t15.txt").build();
	// a, b, c, END = 4
	return assert_true(it.size() == 4, "test_build_returns_iterator_with_correct_size");
}

int test_build_on_empty_file_yields_empty_iterator()
{
	write_file("/tmp/lb_t16.txt", "");
	LexerIterator it = LexerBuilder().withFile("/tmp/lb_t16.txt").build();
	return assert_true(it.size() == 0, "test_build_on_empty_file_yields_empty_iterator");
}

int test_build_on_empty_file_reaches_dangerous_precondition_BUG()
{
	write_file("/tmp/lb_t17.txt", "");
	LexerIterator it = LexerBuilder().withFile("/tmp/lb_t17.txt").build();
	bool dangerous_precondition = (it.size() == 0) && !(bool)it;
	return assert_true(dangerous_precondition,
					"test_build_on_empty_file_reaches_dangerous_precondition_BUG");
}

// ── main ──────────────────────────────────────────────────────────────────────

int main(int argc, char **argv)
{
	messages::settup(argc, argv);

	int failures = 0;

	// std::cout << "\n=== withFile() ===\n";
	failures += test_withFile_loads_target_file();
	failures += test_withFile_returns_self_for_chaining();
	failures += test_missing_file_propagates_error_to_iterator();

	// std::cout << "\n=== withDefaultTokens() ===\n";
	failures += test_withDefaultTokens_recognizes_braces();
	failures += test_withDefaultTokens_recognizes_semicolon();
	failures += test_withDefaultTokens_recognizes_double_quote();
	failures += test_withDefaultTokens_recognizes_single_quote();
	failures += test_withDefaultTokens_recognizes_hash_comment();
	failures += test_withDefaultTokens_returns_self_for_chaining();

	// std::cout << "\n=== withComment() ===\n";
	failures += test_withComment_custom_prefix();
	failures += test_withComment_returns_self_for_chaining();

	// std::cout << "\n=== withQuote() ===\n";
	failures += test_withQuote_custom_delimiter();
	failures += test_withQuote_returns_self_for_chaining();
	failures += test_withQuote_multiple_distinct_delimiters();

	// std::cout << "\n=== withKeyword() ===\n";
	failures += test_withKeyword_custom_delimiter();
	failures += test_withKeyword_returns_self_for_chaining();
	failures += test_withKeyword_multiple_distinct();

	// std::cout << "\n=== method chaining completo ===\n";
	failures += test_full_chain_combines_all_configs();
	failures += test_chain_order_does_not_matter();

	// std::cout << "\n=== build() ===\n";
	failures += test_build_returns_iterator_with_correct_size();
	failures += test_build_on_empty_file_yields_empty_iterator();

	// std::cout << "\n=== BUG: build() em arquivo vazio gera pré-condição perigosa ===\n";
	failures += test_build_on_empty_file_reaches_dangerous_precondition_BUG();

		messages::print();

	return failures;
}
