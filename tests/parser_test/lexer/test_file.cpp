/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_file.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/19 16:42:17 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/19 16:42:45 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "test_utils.hpp"
#include "File.hpp"

#include <fstream>
#include <cstdio>  // std::remove

// ── fixtures ─────────────────────────────────────────────────────────────────

static const char *HELLO = "/tmp/file_test_hello.txt";   // "hello\nworld\nfoo\n"
static const char *EMPTY = "/tmp/file_test_empty.txt";   // vazio
static const char *MISSING = "/tmp/file_test_no_such.txt";

static void setup_fixtures()
{
	{
		std::ofstream f(HELLO);
		f << "hello\nworld\nfoo\n";
	}
	{
		std::ofstream f(EMPTY);
		// nada — arquivo vazio
	}
	std::remove(MISSING); // garante que não existe
}

// helper: avança n posições
static File advance(const char *path, size_t n)
{
	File f(path);
	f += n;
	return f;
}

// ── Construtor com nome + getters básicos ─────────────────────────────────────

int test_ctor_name_reads_file()
{
	File f(HELLO);
	return assert_true(!f.hasError() && f.size() == 16, "test_ctor_name_reads_file", LINE_DATA());
}

int test_ctor_name_initial_cursor()
{
	File f(HELLO);
	return assert_true(f.cursor() == 0, "test_ctor_name_initial_cursor", LINE_DATA());
}

int test_ctor_name_initial_line()
{
	File f(HELLO);
	return assert_true(f.line() == 1, "test_ctor_name_initial_line", LINE_DATA());
}

int test_ctor_name_initial_col()
{
	File f(HELLO);
	return assert_true(f.lineColumn() == 0, "test_ctor_name_initial_col", LINE_DATA());
}

int test_ctor_name_stores_name()
{
	File f(HELLO);
	return assert_true(f.name() == HELLO, "test_ctor_name_stores_name", LINE_DATA());
}

int test_ctor_default_is_empty()
{
	File f;
	return assert_true(!f.hasError() && f.size() == 0, "test_ctor_default_is_empty", LINE_DATA());
}

int test_ctor_default_initial_line()
{
	// Construtor default também inicializa _line=1 (consistente com o construtor com nome)
	File f;
	return assert_true(f.line() == 1, "test_ctor_default_initial_line", LINE_DATA());
}

// ── operator bool ─────────────────────────────────────────────────────────────

int test_bool_true_when_chars_remain()
{
	File f(HELLO);
	return assert_true((bool)f == true, "test_bool_true_when_chars_remain", LINE_DATA());
}

int test_bool_false_at_end()
{
	File f(HELLO);
	f += 100;
	return assert_true((bool)f == false, "test_bool_false_at_end", LINE_DATA());
}

int test_bool_false_empty_file()
{
	File f(EMPTY);
	return assert_true((bool)f == false, "test_bool_false_empty_file", LINE_DATA());
}

int test_bool_false_missing_file()
{
	File f(MISSING);
	return assert_true((bool)f == false, "test_bool_false_missing_file", LINE_DATA());
}

// ── Erros de leitura ──────────────────────────────────────────────────────────

int test_missing_file_has_error()
{
	File f(MISSING);
	return assert_true(f.hasError(), "test_missing_file_has_error", LINE_DATA());
}

int test_missing_file_error_message()
{
	File f(MISSING);
	bool ok = f.error().find("Cannot open file") != std::string::npos;
	return assert_true(ok, "test_missing_file_error_message", LINE_DATA());
}

int test_missing_file_size_zero()
{
	File f(MISSING);
	return assert_true(f.size() == 0, "test_missing_file_size_zero", LINE_DATA());
}

int test_valid_file_no_error()
{
	File f(HELLO);
	return assert_true(!f.hasError() && f.error().empty(), "test_valid_file_no_error", LINE_DATA());
}

// ── operator* (dereference) ───────────────────────────────────────────────────

int test_deref_first_char()
{
	File f(HELLO);
	return assert_true(*f == 'h', "test_deref_first_char", LINE_DATA());
}

int test_deref_after_advance()
{
	File f(HELLO);
	++f; ++f;
	return assert_true(*f == 'l', "test_deref_after_advance", LINE_DATA());
}

// ── operator++ (pré-incremento) ───────────────────────────────────────────────

int test_preinc_advances_cursor()
{
	File f(HELLO);
	++f;
	return assert_true(f.cursor() == 1, "test_preinc_advances_cursor", LINE_DATA());
}

int test_preinc_advances_col()
{
	File f(HELLO);
	++f; ++f; ++f;
	return assert_true(f.lineColumn() == 3, "test_preinc_advances_col", LINE_DATA());
}

int test_preinc_crosses_newline_advances_line()
{
	// "hello\n" → cursor=5 é '\n', avançar muda para linha 2
	File f = advance(HELLO, 5); // on '\n'
	++f;
	return assert_true(f.line() == 2 && f.lineColumn() == 0,
					"test_preinc_crosses_newline_advances_line", LINE_DATA());
}

int test_preinc_at_end_is_noop()
{
	File f(HELLO);
	f += 100; // cursor = size
	size_t cur_before = f.cursor();
	++f;
	return assert_true(f.cursor() == cur_before, "test_preinc_at_end_is_noop", LINE_DATA());
}

int test_preinc_returns_self_reference()
{
	File f(HELLO);
	File &ref = ++f;
	return assert_true(&ref == &f, "test_preinc_returns_self_reference", LINE_DATA());
}

// ── operator++ (pós-incremento) ───────────────────────────────────────────────

int test_postinc_returns_old_state()
{
	File f(HELLO);
	File copy = f++;
	return assert_true(copy.cursor() == 0 && f.cursor() == 1,
					"test_postinc_returns_old_state", LINE_DATA());
}

int test_postinc_on_newline_returns_old_state()
{
	File f = advance(HELLO, 5); // on '\n'
	size_t before_cursor = f.cursor();
	File copy = f++;
	return assert_true(copy.cursor() == before_cursor && f.cursor() == before_cursor + 1,
					"test_postinc_on_newline_returns_old_state", LINE_DATA());
}

// ── operator-- (pré-decremento) ───────────────────────────────────────────────

int test_predec_retreats_cursor()
{
	File f = advance(HELLO, 3);
	--f;
	return assert_true(f.cursor() == 2, "test_predec_retreats_cursor", LINE_DATA());
}

int test_predec_retreats_col()
{
	File f = advance(HELLO, 3);
	--f;
	return assert_true(f.lineColumn() == 2, "test_predec_retreats_col", LINE_DATA());
}

int test_predec_crosses_newline_back()
{
	// Avança para 'w' de "world" (idx=6, line=2), retrocede de volta ao '\n'
	File f = advance(HELLO, 6);
	--f;
	// Volta ao '\n' em idx=5: line=1, col=len("world")=5
	return assert_true(f.cursor() == 5 && f.line() == 1 && f.lineColumn() == 5,
					"test_predec_crosses_newline_back", LINE_DATA());
}

int test_predec_at_zero_is_noop()
{
	File f(HELLO);
	--f;
	return assert_true(f.cursor() == 0 && f.line() == 1, "test_predec_at_zero_is_noop", LINE_DATA());
}

int test_predec_returns_self_reference()
{
	File f = advance(HELLO, 3);
	File &ref = --f;
	return assert_true(&ref == &f, "test_predec_returns_self_reference", LINE_DATA());
}

// ── operator-- (pós-decremento) ───────────────────────────────────────────────

int test_postdec_returns_old_state()
{
	File f = advance(HELLO, 3);
	File copy = f--;
	return assert_true(copy.cursor() == 3 && f.cursor() == 2,
					"test_postdec_returns_old_state", LINE_DATA());
}

int test_postdec_on_newline_returns_old_state()
{
	File f = advance(HELLO, 5); // on '\n'
	size_t before = f.cursor();
	File copy = f--;
	return assert_true(copy.cursor() == before && f.cursor() == before - 1,
					"test_postdec_on_newline_returns_old_state", LINE_DATA());
}

// ── operator+= ────────────────────────────────────────────────────────────────

int test_pluseq_advances_multiple()
{
	File f(HELLO);
	f += 3;
	return assert_true(f.cursor() == 3 && *f == 'l', "test_pluseq_advances_multiple", LINE_DATA());
}

int test_pluseq_crosses_newline()
{
	File f(HELLO);
	f += 6; // crosses '\n' at idx=5, lands on 'w' in "world"
	return assert_true(f.cursor() == 6 && f.line() == 2 && f.lineColumn() == 0,
					"test_pluseq_crosses_newline", LINE_DATA());
}

int test_pluseq_past_end_clamps()
{
	File f(HELLO);
	f += 1000;
	return assert_true(f.cursor() == f.size() && !(bool)f, "test_pluseq_past_end_clamps", LINE_DATA());
}

int test_pluseq_zero_is_noop()
{
	File f(HELLO);
	f += 0;
	return assert_true(f.cursor() == 0, "test_pluseq_zero_is_noop", LINE_DATA());
}

// ── operator-= ────────────────────────────────────────────────────────────────

int test_minuseq_retreats_multiple()
{
	File f = advance(HELLO, 6); // on 'w', line=2
	f -= 3;
	// retreat 3: 6->5('\n')->4->3, crossing back over '\n'
	return assert_true(f.cursor() == 3 && f.line() == 1,
					"test_minuseq_retreats_multiple", LINE_DATA());
}

int test_minuseq_past_start_clamps()
{
	File f = advance(HELLO, 3);
	f -= 100;
	return assert_true(f.cursor() == 0 && f.line() == 1, "test_minuseq_past_start_clamps", LINE_DATA());
}

int test_minuseq_zero_is_noop()
{
	File f = advance(HELLO, 3);
	f -= 0;
	return assert_true(f.cursor() == 3, "test_minuseq_zero_is_noop", LINE_DATA());
}

// ── nextIs ────────────────────────────────────────────────────────────────────

int test_nextIs_match_at_start()
{
	File f(HELLO);
	return assert_true(f.nextIs("hello"), "test_nextIs_match_at_start", LINE_DATA());
}

int test_nextIs_no_match()
{
	File f(HELLO);
	return assert_true(!f.nextIs("world"), "test_nextIs_no_match", LINE_DATA());
}

int test_nextIs_empty_string_always_true()
{
	File f(HELLO);
	return assert_true(f.nextIs(""), "test_nextIs_empty_string_always_true", LINE_DATA());
}

int test_nextIs_after_advance()
{
	File f = advance(HELLO, 6); // on 'w'
	return assert_true(f.nextIs("world"), "test_nextIs_after_advance", LINE_DATA());
}

int test_nextIs_too_long_at_end()
{
	File f = advance(HELLO, 15); // last char '\n', 1 char left
	return assert_true(!f.nextIs("\n!"), "test_nextIs_too_long_at_end", LINE_DATA());
}

int test_nextIs_exact_fit_at_end()
{
	File f = advance(HELLO, 15); // on last '\n'
	return assert_true(f.nextIs("\n"), "test_nextIs_exact_fit_at_end", LINE_DATA());
}

// ── substr_forward ────────────────────────────────────────────────────────────

int test_substr_forward_from_start()
{
	File f(HELLO);
	return assert_true(f.substr_forward(5) == "hello", "test_substr_forward_from_start", LINE_DATA());
}

int test_substr_forward_after_advance()
{
	File f = advance(HELLO, 6); // on 'w'
	return assert_true(f.substr_forward(5) == "world", "test_substr_forward_after_advance", LINE_DATA());
}

int test_substr_forward_no_size_returns_rest()
{
	File f = advance(HELLO, 12); // on 'f' of "foo"
	return assert_true(f.substr_forward() == "foo\n", "test_substr_forward_no_size_returns_rest", LINE_DATA());
}

// ── substr_back ───────────────────────────────────────────────────────────────

int test_substr_back_pos_equal_cursor_returns_empty()
{
	File f(HELLO); // cursor=0
	return assert_true(f.substr_back(0) == "", "test_substr_back_pos_equal_cursor_returns_empty", LINE_DATA());
}

int test_substr_back_includes_cursor_char()
{
	// cursor=5 (on '\n'), pos=0 -> substr(0, 5-0+1=6) = "hello\n"
	File f = advance(HELLO, 5);
	return assert_true(f.substr_back(0) == "hello\n",
					"test_substr_back_includes_cursor_char", LINE_DATA());
}

int test_substr_back_mid_range()
{
	File f = advance(HELLO, 8); // on 'l' of "world", idx=8
	// pos=6 ('w'): substr(6, 8-6+1=3) = "wor"
	return assert_true(f.substr_back(6) == "wor", "test_substr_back_mid_range", LINE_DATA());
}

int test_substr_back_pos_greater_than_cursor_returns_empty()
{
	File f = advance(HELLO, 3); // cursor=3
	return assert_true(f.substr_back(5) == "", "test_substr_back_pos_greater_than_cursor_returns_empty", LINE_DATA());
}

// ── substr ────────────────────────────────────────────────────────────────────

int test_substr_pos_before_cursor()
{
	File f = advance(HELLO, 5); // cursor=5
	return assert_true(f.substr(0, 5) == "hello", "test_substr_pos_before_cursor", LINE_DATA());
}

int test_substr_pos_equal_cursor_returns_empty()
{
	File f = advance(HELLO, 3); // cursor=3
	return assert_true(f.substr(3, 1) == "", "test_substr_pos_equal_cursor_returns_empty", LINE_DATA());
}

int test_substr_pos_after_cursor_returns_empty()
{
	File f = advance(HELLO, 3); // cursor=3
	return assert_true(f.substr(5, 1) == "", "test_substr_pos_after_cursor_returns_empty", LINE_DATA());
}

int test_substr_no_size_returns_from_pos_to_end()
{
	File f = advance(HELLO, 5); // cursor=5
	// pos=0 < 5 -> substr(0, npos) = "hello\nworld\nfoo\n"
	return assert_true(f.substr(0) == "hello\nworld\nfoo\n",
					"test_substr_no_size_returns_from_pos_to_end", LINE_DATA());
}

// ── lineContentAt ─────────────────────────────────────────────────────────────

int test_lineContentAt_index_zero()
{
	File f(HELLO);
	const std::string *l = f.lineContentAt(0);
	return assert_true(l != NULL && *l == "hello", "test_lineContentAt_index_zero", LINE_DATA());
}

int test_lineContentAt_index_one()
{
	File f(HELLO);
	const std::string *l = f.lineContentAt(1);
	return assert_true(l != NULL && *l == "world", "test_lineContentAt_index_one", LINE_DATA());
}

int test_lineContentAt_index_two()
{
	File f(HELLO);
	const std::string *l = f.lineContentAt(2);
	return assert_true(l != NULL && *l == "foo", "test_lineContentAt_index_two", LINE_DATA());
}

int test_lineContentAt_out_of_range_returns_null()
{
	File f(HELLO);
	return assert_true(f.lineContentAt(99) == NULL, "test_lineContentAt_out_of_range_returns_null", LINE_DATA());
}

int test_lineContent_at_start()
{
	File f(HELLO);
	const std::string *lc = f.lineContent();
	int errors = 0;
	errors += assert_true(lc != NULL, "test_lineContent_at_start", LINE_DATA());
	if (errors) return errors;
	errors += assert(std::string("hello"), *lc, "test_lineContent_at_start", LINE_DATA());
	return errors;
}

int test_lineContent_on_second_line()
{
	File f = advance(HELLO, 6);
	const std::string *lc = f.lineContent();
	int errors = 0;
	errors += assert_true(lc != NULL, "test_lineContent_on_second_line", LINE_DATA());
	if (errors) return errors;
	errors += assert(std::string("world"), *lc, "test_lineContent_on_second_line", LINE_DATA());
	return errors;
}

int test_lineContent_returns_last_line()
{
	File f = advance(HELLO, 12);
	const std::string *lc = f.lineContent();
	int errors = 0;
	errors += assert_true(lc != NULL, "test_lineContent_returns_last_line", LINE_DATA());
	if (errors) return errors;
	errors += assert(std::string("foo"), *lc, "test_lineContent_returns_last_line", LINE_DATA());
	return errors;
}

int test_setName_loads_new_file()
{
	File f;
	f.setName(HELLO);
	return assert_true(!f.hasError() && f.size() == 16, "test_setName_loads_new_file", LINE_DATA());
}

int test_setName_resets_cursor()
{
	File f = advance(HELLO, 6);
	f.setName(HELLO);
	return assert_true(f.cursor() == 0, "test_setName_resets_cursor", LINE_DATA());
}

int test_setName_line_is_one()
{
	File f = advance(HELLO, 6);
	f.setName(HELLO);
	return assert_true(f.line() == 1, "test_setName_line_is_one", LINE_DATA());
}

int test_setName_stores_new_name()
{
	File f(HELLO);
	f.setName(EMPTY);
	return assert_true(f.name() == EMPTY, "test_setName_stores_new_name", LINE_DATA());
}

// ── run() ────────────────────────────────────────────────────────────────────

int test_run_idempotent_when_already_loaded()
{
	File f(HELLO);
	f += 3;
	size_t before = f.cursor();
	f.run();
	return assert_true(f.cursor() == before && f.size() == 16,
					"test_run_idempotent_when_already_loaded", LINE_DATA());
}

int test_run_on_default_with_no_name_sets_error()
{
	// Construtor default deixa _name vazio. run() chama _readFile() que tenta
	// abrir "", falha e seta _error.
	File f;
	f.run();
	return assert_true(f.hasError(), "test_run_on_default_with_no_name_sets_error", LINE_DATA());
}

// ── arquivo vazio ─────────────────────────────────────────────────────────────

int test_empty_file_size_zero()
{
	File f(EMPTY);
	return assert_true(f.size() == 0 && !f.hasError(), "test_empty_file_size_zero", LINE_DATA());
}

int test_empty_file_bool_false()
{
	File f(EMPTY);
	return assert_true(!(bool)f, "test_empty_file_bool_false", LINE_DATA());
}

int test_empty_file_lineContentAt_zero_null()
{
	File f(EMPTY);
	return assert_true(f.lineContentAt(0) == NULL, "test_empty_file_lineContentAt_zero_null", LINE_DATA());
}

// ── operator-> ───────────────────────────────────────────────────────────────

int test_arrow_same_as_deref()
{
	// operator-> é implementado como retorno de char& (igual a operator*),
	// não é um "pointer-to-member" real. Deve retornar o mesmo char.
	File f(HELLO);
	return assert_true(f.operator->() == *f, "test_arrow_same_as_deref", LINE_DATA());
}

// ── main ──────────────────────────────────────────────────────────────────────

int main(int argc, char **argv)
{
	messages::settup(argc, argv);

	setup_fixtures();
	int failures = 0;

	// std::cout << "\n=== Construtor + getters básicos ===\n";
	failures += test_ctor_name_reads_file();
	failures += test_ctor_name_initial_cursor();
	failures += test_ctor_name_initial_line();
	failures += test_ctor_name_initial_col();
	failures += test_ctor_name_stores_name();
	failures += test_ctor_default_is_empty();
	failures += test_ctor_default_initial_line();

	// std::cout << "\n=== operator bool ===\n";
	failures += test_bool_true_when_chars_remain();
	failures += test_bool_false_at_end();
	failures += test_bool_false_empty_file();
	failures += test_bool_false_missing_file();

	// std::cout << "\n=== Erros de leitura ===\n";
	failures += test_missing_file_has_error();
	failures += test_missing_file_error_message();
	failures += test_missing_file_size_zero();
	failures += test_valid_file_no_error();

	// std::cout << "\n=== operator* ===\n";
	failures += test_deref_first_char();
	failures += test_deref_after_advance();

	// std::cout << "\n=== operator++ (pré) ===\n";
	failures += test_preinc_advances_cursor();
	failures += test_preinc_advances_col();
	failures += test_preinc_crosses_newline_advances_line();
	failures += test_preinc_at_end_is_noop();
	failures += test_preinc_returns_self_reference();

	// std::cout << "\n=== operator++ (pós) ===\n";
	failures += test_postinc_returns_old_state();
	failures += test_postinc_on_newline_returns_old_state();

	// std::cout << "\n=== operator-- (pré) ===\n";
	failures += test_predec_retreats_cursor();
	failures += test_predec_retreats_col();
	failures += test_predec_crosses_newline_back();
	failures += test_predec_at_zero_is_noop();
	failures += test_predec_returns_self_reference();

	// std::cout << "\n=== operator-- (pós) ===\n";
	failures += test_postdec_returns_old_state();
	failures += test_postdec_on_newline_returns_old_state();

	// std::cout << "\n=== operator+= ===\n";
	failures += test_pluseq_advances_multiple();
	failures += test_pluseq_crosses_newline();
	failures += test_pluseq_past_end_clamps();
	failures += test_pluseq_zero_is_noop();

	// std::cout << "\n=== operator-= ===\n";
	failures += test_minuseq_retreats_multiple();
	failures += test_minuseq_past_start_clamps();
	failures += test_minuseq_zero_is_noop();

	// std::cout << "\n=== nextIs ===\n";
	failures += test_nextIs_match_at_start();
	failures += test_nextIs_no_match();
	failures += test_nextIs_empty_string_always_true();
	failures += test_nextIs_after_advance();
	failures += test_nextIs_too_long_at_end();
	failures += test_nextIs_exact_fit_at_end();

	// std::cout << "\n=== substr_forward ===\n";
	failures += test_substr_forward_from_start();
	failures += test_substr_forward_after_advance();
	failures += test_substr_forward_no_size_returns_rest();

	// std::cout << "\n=== substr_back ===\n";
	failures += test_substr_back_pos_equal_cursor_returns_empty();
	failures += test_substr_back_includes_cursor_char();
	failures += test_substr_back_mid_range();
	failures += test_substr_back_pos_greater_than_cursor_returns_empty();

	// std::cout << "\n=== substr ===\n";
	failures += test_substr_pos_before_cursor();
	failures += test_substr_pos_equal_cursor_returns_empty();
	failures += test_substr_pos_after_cursor_returns_empty();
	failures += test_substr_no_size_returns_from_pos_to_end();

	// std::cout << "\n=== lineContentAt ===\n";
	failures += test_lineContentAt_index_zero();
	failures += test_lineContentAt_index_one();
	failures += test_lineContentAt_index_two();
	failures += test_lineContentAt_out_of_range_returns_null();

	// std::cout << "\n=== lineContent() off-by-one ===\n";
	failures += test_lineContent_at_start();
	failures += test_lineContent_on_second_line();
	failures += test_lineContent_returns_last_line();

	// std::cout << "\n=== setName() inicializa _line=0 ===\n";
	failures += test_setName_loads_new_file();
	failures += test_setName_resets_cursor();
	failures += test_setName_line_is_one();
	failures += test_setName_stores_new_name();

	// std::cout << "\n=== run() ===\n";
	failures += test_run_idempotent_when_already_loaded();
	failures += test_run_on_default_with_no_name_sets_error();

	// std::cout << "\n=== arquivo vazio ===\n";
	failures += test_empty_file_size_zero();
	failures += test_empty_file_bool_false();
	failures += test_empty_file_lineContentAt_zero_null();

	// std::cout << "\n=== operator-> ===\n";
	failures += test_arrow_same_as_deref();

	messages::print();

	return failures;
}
