/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_lexer_iterator.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/19 22:27:22 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/19 22:27:25 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "test_utils.hpp"
#include "LexerIterator.hpp"

#include <fstream>

// ── helpers ──────────────────────────────────────────────────────────────────

static void write_file(const std::string &path, const std::string &content)
{
	std::ofstream f(path.c_str());
	f << content;
}

// Cria um Lexer já executado sobre `content`, configurado com um delimitador
// simples ('+') para gerar tokens variados, e devolve a instância.
static Lexer make_lexer(const std::string &path, const std::string &content)
{
	write_file(path, content);
	Lexer lex(path);
	lex.addDelimitter("+", LexerTokenType::DELIM_PLUS);
	lex.run();
	return lex;
}

// ── Construtores ──────────────────────────────────────────────────────────────

int test_ctor_from_lexer_copies_token_count()
{
	Lexer lex = make_lexer("/tmp/li_t1.txt", "a b c");
	LexerIterator it(lex);
	// a, b, c, END = 4
	return assert_true(it.size() == 4, "test_ctor_from_lexer_copies_token_count", LINE_DATA());
}

int test_ctor_from_lexer_copies_error()
{
	write_file("/tmp/li_t2.txt", "\"unterminated");
	Lexer lex("/tmp/li_t2.txt");
	lex.addDelimitter("+", LexerTokenType::DELIM_PLUS); // sem quote registrada -> sem erro de quote
	lex.run();
	LexerIterator it(lex);
	return assert_true(it.error() == lex.error(), "test_ctor_from_lexer_copies_error", LINE_DATA());
}

int test_ctor_default_empty_size()
{
	LexerIterator it;
	return assert_true(it.size() == 0, "test_ctor_default_empty_size", LINE_DATA());
}

int test_ctor_default_empty_error()
{
	LexerIterator it;
	return assert_true(it.error().empty(), "test_ctor_default_empty_error", LINE_DATA());
}

int test_ctor_default_bool_false()
{
	LexerIterator it;
	return assert_true(!(bool)it, "test_ctor_default_bool_false", LINE_DATA());
}

// ── size() / error() ──────────────────────────────────────────────────────────

int test_size_matches_lexer_tokens()
{
	Lexer lex = make_lexer("/tmp/li_t3.txt", "x y");
	LexerIterator it(lex);
	return assert_true(it.size() == lex.tokens().size(), "test_size_matches_lexer_tokens", LINE_DATA());
}

int test_error_empty_when_no_problem()
{
	Lexer lex = make_lexer("/tmp/li_t4.txt", "fine");
	LexerIterator it(lex);
	return assert_true(it.error().empty(), "test_error_empty_when_no_problem", LINE_DATA());
}

// ── operator bool ─────────────────────────────────────────────────────────────

int test_bool_true_at_start_when_tokens_exist()
{
	Lexer lex = make_lexer("/tmp/li_t5.txt", "a");
	LexerIterator it(lex);
	return assert_true((bool)it == true, "test_bool_true_at_start_when_tokens_exist", LINE_DATA());
}

int test_bool_false_after_walking_past_end()
{
	Lexer lex = make_lexer("/tmp/li_t6.txt", "a");
	LexerIterator it(lex);
	while (it) ++it;
	return assert_true((bool)it == false, "test_bool_false_after_walking_past_end", LINE_DATA());
}

// ── operator* (dereference) ───────────────────────────────────────────────────

int test_deref_first_token()
{
	Lexer lex = make_lexer("/tmp/li_t7.txt", "alpha beta");
	LexerIterator it(lex);
	return assert(std::string("alpha"), (*it).getContent(), "test_deref_first_token", LINE_DATA());
}

int test_deref_after_increment()
{
	Lexer lex = make_lexer("/tmp/li_t8.txt", "alpha beta");
	LexerIterator it(lex);
	++it;
	return assert(std::string("beta"), (*it).getContent(), "test_deref_after_increment", LINE_DATA());
}

// Feature: ao avançar o cursor exatamente até size() (uma posição após o
// último token real), operator* devolve o ÚLTIMO token, em vez de um estado
// inválido — esse comportamento é intencional e documentado no header.
int test_deref_past_end_returns_last_token()
{
	Lexer lex = make_lexer("/tmp/li_t9.txt", "only");
	LexerIterator it(lex);
	while (it) ++it; // _idx == size()
	const Lexer::token &last_real = lex.tokens().back();
	return assert_true((*it).getContent() == last_real.getContent()
					&& (*it).getType() == last_real.getType(),
					"test_deref_past_end_returns_last_token");
}

int test_operator_arrow_explicit_call_returns_correct_value()
{
	Lexer lex = make_lexer("/tmp/li_t10.txt", "wordA wordB");
	LexerIterator it(lex);
	const Lexer::token *via_arrow = it.operator->(); // chamada explícita
	return assert(std::string("wordA"), via_arrow->getContent(),
					"test_operator_arrow_explicit_call_returns_correct_value", LINE_DATA());
}

// ── operator++ (pré e pós) ─────────────────────────────────────────────────────

int test_preinc_advances()
{
	Lexer lex = make_lexer("/tmp/li_t11.txt", "a b");
	LexerIterator it(lex);
	++it;
	return assert(std::string("b"), (*it).getContent(), "test_preinc_advances", LINE_DATA());
}

int test_preinc_stops_at_end_no_further_advance()
{
	Lexer lex = make_lexer("/tmp/li_t12.txt", "a");
	LexerIterator it(lex);
	size_t total = it.size();
	for (size_t i = 0; i < total + 5; i++) ++it; // tenta avançar bem mais que size()
	return assert_true(!(bool)it, "test_preinc_stops_at_end_no_further_advance");
}

int test_preinc_returns_self_reference()
{
	Lexer lex = make_lexer("/tmp/li_t13.txt", "a b");
	LexerIterator it(lex);
	LexerIterator &ref = ++it;
	return assert_true(&ref == &it, "test_preinc_returns_self_reference");
}

int test_postinc_returns_old_state()
{
	Lexer lex = make_lexer("/tmp/li_t14.txt", "a b");
	LexerIterator it(lex);
	LexerIterator copy = it++;
	bool ok = (*copy).getContent() == "a" && (*it).getContent() == "b";
	return assert_true(ok, "test_postinc_returns_old_state");
}

// ── operator-- (pré e pós) ─────────────────────────────────────────────────────

int test_predec_retreats()
{
	Lexer lex = make_lexer("/tmp/li_t15.txt", "a b c");
	LexerIterator it(lex);
	++it; ++it; // em 'c'
	--it;
	return assert(std::string("b"), (*it).getContent(), "test_predec_retreats", LINE_DATA());
}

int test_predec_at_zero_is_noop()
{
	Lexer lex = make_lexer("/tmp/li_t16.txt", "a");
	LexerIterator it(lex);
	--it;
	return assert(std::string("a"), (*it).getContent(), "test_predec_at_zero_is_noop", LINE_DATA());
}

int test_predec_returns_self_reference()
{
	Lexer lex = make_lexer("/tmp/li_t17.txt", "a b");
	LexerIterator it(lex);
	++it;
	LexerIterator &ref = --it;
	return assert_true(&ref == &it, "test_predec_returns_self_reference");
}

int test_postdec_returns_old_state()
{
	Lexer lex = make_lexer("/tmp/li_t18.txt", "a b");
	LexerIterator it(lex);
	++it; // em 'b'
	LexerIterator copy = it--;
	bool ok = (*copy).getContent() == "b" && (*it).getContent() == "a";
	return assert_true(ok, "test_postdec_returns_old_state");
}

// ── operator+= ────────────────────────────────────────────────────────────────

int test_pluseq_advances_multiple()
{
	Lexer lex = make_lexer("/tmp/li_t19.txt", "a b c d");
	LexerIterator it(lex);
	it += 2;
	return assert(std::string("c"), (*it).getContent(), "test_pluseq_advances_multiple", LINE_DATA());
}

int test_pluseq_past_end_clamps_to_last()
{
	Lexer lex = make_lexer("/tmp/li_t20.txt", "a b");
	LexerIterator it(lex);
	it += 1000;
	return assert_true(!(bool)it, "test_pluseq_past_end_clamps_to_last");
}

int test_pluseq_zero_is_noop()
{
	Lexer lex = make_lexer("/tmp/li_t21.txt", "a b");
	LexerIterator it(lex);
	it += 0;
	return assert(std::string("a"), (*it).getContent(), "test_pluseq_zero_is_noop", LINE_DATA());
}

// ── operator-= ────────────────────────────────────────────────────────────────

int test_minuseq_retreats_multiple()
{
	Lexer lex = make_lexer("/tmp/li_t22.txt", "a b c d");
	LexerIterator it(lex);
	it += 3; // em 'd'
	it -= 2;
	return assert(std::string("b"), (*it).getContent(), "test_minuseq_retreats_multiple", LINE_DATA());
}

int test_minuseq_past_start_clamps_to_zero()
{
	Lexer lex = make_lexer("/tmp/li_t23.txt", "a b c");
	LexerIterator it(lex);
	it += 1;
	it -= 1000;
	return assert(std::string("a"), (*it).getContent(), "test_minuseq_past_start_clamps_to_zero", LINE_DATA());
}

int test_minuseq_zero_is_noop()
{
	Lexer lex = make_lexer("/tmp/li_t24.txt", "a b");
	LexerIterator it(lex);
	it += 1;
	it -= 0;
	return assert(std::string("b"), (*it).getContent(), "test_minuseq_zero_is_noop", LINE_DATA());
}

// ── operator= (a partir de um Lexer) ──────────────────────────────────────────

int test_assign_from_lexer_replaces_tokens()
{
	Lexer lex1 = make_lexer("/tmp/li_t25a.txt", "first");
	Lexer lex2 = make_lexer("/tmp/li_t25b.txt", "second");
	LexerIterator it(lex1);
	it = lex2;
	return assert(std::string("second"), (*it).getContent(), "test_assign_from_lexer_replaces_tokens", LINE_DATA());
}

int test_assign_from_lexer_resets_idx()
{
	Lexer lex1 = make_lexer("/tmp/li_t26a.txt", "a b c d");
	Lexer lex2 = make_lexer("/tmp/li_t26b.txt", "x y");

	LexerIterator it(lex1);
	it += 10;
	it = lex2;
	int errors = 0;
	errors = assert(LexerTokenType::WORD, it->getType(), "test_assign_from_lexer_resets_idx", LINE_DATA());
	errors = assert(std::string("x"), it->getContent(), "test_assign_from_lexer_resets_idx", LINE_DATA());
	return errors;
}

int test_deref_empty_iterator_should_never_crash()
{
	LexerIterator it;

	const LexerIterator::token* ptr = it.operator->();

	int errors = 0;
	errors = assert(static_cast<const LexerIterator::token*>(NULL), ptr, "test_deref_empty_iterator_should_return_null", LINE_DATA());

	return errors;
}

// ── main ──────────────────────────────────────────────────────────────────────

int main(int argc, char **argv)
{
	messages::settup(argc, argv);

	int failures = 0;

	// std::cout << "\n=== Construtores ===\n";
	failures += test_ctor_from_lexer_copies_token_count();
	failures += test_ctor_from_lexer_copies_error();
	failures += test_ctor_default_empty_size();
	failures += test_ctor_default_empty_error();
	failures += test_ctor_default_bool_false();

	// std::cout << "\n=== size() / error() ===\n";
	failures += test_size_matches_lexer_tokens();
	failures += test_error_empty_when_no_problem();

	// std::cout << "\n=== operator bool ===\n";
	failures += test_bool_true_at_start_when_tokens_exist();
	failures += test_bool_false_after_walking_past_end();

	// std::cout << "\n=== operator* ===\n";
	failures += test_deref_first_token();
	failures += test_deref_after_increment();
	failures += test_deref_past_end_returns_last_token();

	// std::cout << "\n=== BUG: operator-> não retorna ponteiro ===\n";
	failures += test_operator_arrow_explicit_call_returns_correct_value();

	// std::cout << "\n=== operator++ (pré/pós) ===\n";
	failures += test_preinc_advances();
	failures += test_preinc_stops_at_end_no_further_advance();
	failures += test_preinc_returns_self_reference();
	failures += test_postinc_returns_old_state();

	// std::cout << "\n=== operator-- (pré/pós) ===\n";
	failures += test_predec_retreats();
	failures += test_predec_at_zero_is_noop();
	failures += test_predec_returns_self_reference();
	failures += test_postdec_returns_old_state();

	// std::cout << "\n=== operator+= ===\n";
	failures += test_pluseq_advances_multiple();
	failures += test_pluseq_past_end_clamps_to_last();
	failures += test_pluseq_zero_is_noop();

	// std::cout << "\n=== operator-= ===\n";
	failures += test_minuseq_retreats_multiple();
	failures += test_minuseq_past_start_clamps_to_zero();
	failures += test_minuseq_zero_is_noop();

	// std::cout << "\n=== operator= (a partir de Lexer) ===\n";
	failures += test_assign_from_lexer_replaces_tokens();
	failures += test_assign_from_lexer_resets_idx();

	// std::cout << "\n=== BUG CRÍTICO: deref em iterador vazio (crash) ===\n";
	failures += test_deref_empty_iterator_should_never_crash();

	messages::print();

	return failures;
}
