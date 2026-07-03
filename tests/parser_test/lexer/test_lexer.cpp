/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_lexer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/19 20:51:14 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/19 20:51:20 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "test_utils.hpp"
#include "Lexer.hpp"

#include <fstream>
#include <cstdio>

typedef Lexer::token       token;
typedef Lexer::token_type  token_type;

// ── helpers ──────────────────────────────────────────────────────────────────

static void write_file(const std::string &path, const std::string &content)
{
	std::ofstream f(path.c_str());
	f << content;
}

// Roda o Lexer sobre `content` usando um arquivo temporário único por teste.
static Lexer run_on(const std::string &path, const std::string &content,
					void (*configure)(Lexer &) = NULL)
{
	write_file(path, content);
	Lexer lex(path);
	if (configure) configure(lex);
	lex.run();
	return lex;
}

// ── Construtor / estado inicial ───────────────────────────────────────────────

int test_ctor_default_no_tokens()
{
	Lexer lex;
	return assert_true(lex.tokens().empty(), "test_ctor_default_no_tokens", LINE_DATA());
}

int test_ctor_default_no_error_before_run()
{
	Lexer lex;
	return assert_true(lex.error().empty(), "test_ctor_default_no_error_before_run", LINE_DATA());
}

int test_ctor_with_filename_no_tokens_before_run()
{
	write_file("/tmp/lexer_t_ctor.txt", "hello");
	Lexer lex("/tmp/lexer_t_ctor.txt");
	return assert_true(lex.tokens().empty(), "test_ctor_with_filename_no_tokens_before_run", LINE_DATA());
}

// ── run() — palavras simples ──────────────────────────────────────────────────

int test_run_single_word()
{
	Lexer lex = run_on("/tmp/lexer_t1.txt", "hello");
	bool ok = lex.tokens().size() >= 1
		&& lex.tokens()[0].getType() == LexerTokenType::WORD
		&& lex.tokens()[0].getContent() == "hello";
	return assert_true(ok, "test_run_single_word", LINE_DATA());
}

int test_run_multiple_words_separated_by_space()
{
	Lexer lex = run_on("/tmp/lexer_t2.txt", "hello world foo");
	const std::vector<token> &t = lex.tokens();
	bool ok = t.size() >= 3
		&& t[0].getContent() == "hello" && t[0].getType() == LexerTokenType::WORD
		&& t[1].getContent() == "world" && t[1].getType() == LexerTokenType::WORD
		&& t[2].getContent() == "foo"   && t[2].getType() == LexerTokenType::WORD;
	return assert_true(ok, "test_run_multiple_words_separated_by_space", LINE_DATA());
}

// Feature: o último token emitido é sempre END
int test_run_ends_with_end_token()
{
	Lexer lex = run_on("/tmp/lexer_t3.txt", "hello world");
	const std::vector<token> &t = lex.tokens();
	bool ok = !t.empty() && t.back().getType() == LexerTokenType::END;
	return assert_true(ok, "test_run_ends_with_end_token", LINE_DATA());
}

// Feature: arquivo só com espaços em branco gera apenas o token END
int test_run_only_whitespace_yields_only_end()
{
	Lexer lex = run_on("/tmp/lexer_t4.txt", "   \n\t  ");
	const std::vector<token> &t = lex.tokens();
	bool ok = t.size() == 1 && t[0].getType() == LexerTokenType::END;
	return assert_true(ok, "test_run_only_whitespace_yields_only_end", LINE_DATA());
}

// Feature: múltiplos espaços/tabs/newlines entre palavras são ignorados
int test_run_multiple_whitespace_between_words()
{
	Lexer lex = run_on("/tmp/lexer_t5.txt", "foo   \n\t  bar");
	const std::vector<token> &t = lex.tokens();
	bool ok = t.size() >= 2 && t[0].getContent() == "foo" && t[1].getContent() == "bar";
	return assert_true(ok, "test_run_multiple_whitespace_between_words", LINE_DATA());
}

// ── run() — arquivo vazio / inexistente ───────────────────────────────────────

int test_run_empty_file_no_tokens()
{
	Lexer lex = run_on("/tmp/lexer_t6.txt", "");
	return assert_true(lex.tokens().empty(), "test_run_empty_file_no_tokens", LINE_DATA());
}

int test_run_empty_file_no_error()
{
	Lexer lex = run_on("/tmp/lexer_t7.txt", "");
	return assert_true(lex.error().empty(), "test_run_empty_file_no_error", LINE_DATA());
}

int test_run_missing_file_sets_error()
{
	std::remove("/tmp/lexer_t_missing.txt");
	Lexer lex("/tmp/lexer_t_missing.txt");
	lex.run();
	bool ok = !lex.error().empty()
		&& lex.error().find("Cannot open file") != std::string::npos;
	return assert_true(ok, "test_run_missing_file_sets_error", LINE_DATA());
}

int test_run_missing_file_no_tokens()
{
	std::remove("/tmp/lexer_t_missing2.txt");
	Lexer lex("/tmp/lexer_t_missing2.txt");
	lex.run();
	return assert_true(lex.tokens().empty(), "test_run_missing_file_no_tokens", LINE_DATA());
}

// ── setFile() ─────────────────────────────────────────────────────────────────

int test_setFile_switches_target()
{
	write_file("/tmp/lexer_t8a.txt", "first");
	write_file("/tmp/lexer_t8b.txt", "second");
	Lexer lex("/tmp/lexer_t8a.txt");
	lex.setFile("/tmp/lexer_t8b.txt");
	lex.run();
	const std::vector<token> &t = lex.tokens();
	bool ok = !t.empty() && t[0].getContent() == "second";
	return assert_true(ok, "test_setFile_switches_target", LINE_DATA());
}

// ── addDelimitter() ───────────────────────────────────────────────────────────

int test_delimitter_single_char()
{
	Lexer lex = run_on("/tmp/lexer_t9.txt", "a+b", NULL);
	write_file("/tmp/lexer_t9.txt", "a+b");
	Lexer lex2("/tmp/lexer_t9.txt");
	lex2.addDelimitter("+", LexerTokenType::DELIM_PLUS);
	lex2.run();
	const std::vector<token> &t = lex2.tokens();
	bool ok = t.size() >= 3
		&& t[0].getContent() == "a" && t[0].getType() == LexerTokenType::WORD
		&& t[1].getContent() == "+" && t[1].getType() == LexerTokenType::DELIM_PLUS
		&& t[2].getContent() == "b" && t[2].getType() == LexerTokenType::WORD;
	return assert_true(ok, "test_delimitter_single_char", LINE_DATA());
}

int test_delimitter_multi_char()
{
	write_file("/tmp/lexer_t10.txt", "a->b");
	Lexer lex("/tmp/lexer_t10.txt");
	lex.addDelimitter("->", LexerTokenType::DELIM_ARROW);
	lex.run();
	const std::vector<token> &t = lex.tokens();
	bool ok = t.size() >= 3
		&& t[0].getContent() == "a"  && t[0].getType() == LexerTokenType::WORD
		&& t[1].getContent() == "->" && t[1].getType() == LexerTokenType::DELIM_ARROW
		&& t[2].getContent() == "b"  && t[2].getType() == LexerTokenType::WORD;
	return assert_true(ok, "test_delimitter_multi_char", LINE_DATA());
}

int test_delimitter_adjacent_to_word_no_space()
{
	write_file("/tmp/lexer_t11.txt", "foo+bar");
	Lexer lex("/tmp/lexer_t11.txt");
	lex.addDelimitter("+", LexerTokenType::DELIM_PLUS);
	lex.run();
	const std::vector<token> &t = lex.tokens();
	bool ok = t.size() >= 3
		&& t[0].getContent() == "foo" && t[1].getContent() == "+" && t[2].getContent() == "bar";
	return assert_true(ok, "test_delimitter_adjacent_to_word_no_space", LINE_DATA());
}

int test_delimitter_multiple_distinct()
{
	write_file("/tmp/lexer_t12.txt", "a;b=c");
	Lexer lex("/tmp/lexer_t12.txt");
	lex.addDelimitter(";", LexerTokenType::DELIM_SEMI);
	lex.addDelimitter("=", LexerTokenType::DELIM_ASSIGN);
	lex.run();
	const std::vector<token> &t = lex.tokens();
	bool ok = t.size() >= 5
		&& t[0].getContent() == "a" && t[1].getContent() == ";"
		&& t[2].getContent() == "b" && t[3].getContent() == "=" && t[4].getContent() == "c";
	return assert_true(ok, "test_delimitter_multiple_distinct", LINE_DATA());
}


int test_delimitter_empty_string_is_false()
{
	write_file("/tmp/lexer_t13.txt", "ab");
	File f("/tmp/lexer_t13.txt");
	bool ok = f.nextIs("");
	return assert_false(ok, "test_delimitter_empty_string_is_false", LINE_DATA());
}

// ── addComment() ──────────────────────────────────────────────────────────────

int test_comment_basic()
{
	write_file("/tmp/lexer_t14.txt", "foo # a comment\nbar");
	Lexer lex("/tmp/lexer_t14.txt");
	lex.addComment("#");
	lex.run();
	const std::vector<token> &t = lex.tokens();
	bool ok = t.size() >= 3
		&& t[0].getContent() == "foo" && t[0].getType() == LexerTokenType::WORD
		&& t[1].getType() == LexerTokenType::COMMENT
		&& t[2].getContent() == "bar" && t[2].getType() == LexerTokenType::WORD;
	return assert_true(ok, "test_comment_basic", LINE_DATA());
}

// Feature: o comentário captura o texto desde o prefixo até (e incluindo,
// ver bug abaixo) o fim da linha
int test_comment_captures_prefix_to_text()
{
	write_file("/tmp/lexer_t15.txt", "# hello\nbar");
	Lexer lex("/tmp/lexer_t15.txt");
	lex.addComment("#");
	lex.run();
	const std::vector<token> &t = lex.tokens();
	bool ok = !t.empty() && t[0].getType() == LexerTokenType::COMMENT
		&& t[0].getContent().find("# hello") == 0;
	return assert_true(ok, "test_comment_captures_prefix_to_text", LINE_DATA());
}

int test_comment_dont_includes_trailing_newline()
{
	write_file("/tmp/lexer_t16.txt", "# hello\nbar");
	Lexer lex("/tmp/lexer_t16.txt");
	lex.addComment("#");
	lex.run();
	const std::vector<token> &t = lex.tokens();
	bool ok = !t.empty() && t[0].getType() == LexerTokenType::COMMENT
		&& t[0].getContent() == "# hello";
	return assert_true(ok, "test_comment_dont_includes_trailing_newline", LINE_DATA());
}

// Feature: comentário no final do arquivo sem newline explícito ainda
// funciona (File adiciona '\n' ao final de cada linha lida via getline)
int test_comment_at_end_of_file_without_trailing_newline()
{
	write_file("/tmp/lexer_t17.txt", "foo # comment no newline");
	Lexer lex("/tmp/lexer_t17.txt");
	lex.addComment("#");
	lex.run();
	const std::vector<token> &t = lex.tokens();
	bool ok = t.size() >= 2 && t[1].getType() == LexerTokenType::COMMENT;
	return assert_true(ok, "test_comment_at_end_of_file_without_trailing_newline", LINE_DATA());
}


int test_comment_prefix_adjacent_to_word_is_swallowed()
{
	write_file("/tmp/lexer_t18.txt", "foo#bar");
	Lexer lex("/tmp/lexer_t18.txt");
	lex.addComment("#");
	lex.run();
	const std::vector<token> &t = lex.tokens();
	int errors = 0;
	errors += assert_true(!t.empty(), "test_comment_prefix_adjacent_to_word_is_swallowed", LINE_DATA());
	errors += assert(std::string("foo"), t[0].getContent(), "test_comment_prefix_adjacent_to_word_is_swallowed", LINE_DATA());
	errors += assert(LexerTokenType::WORD, t[0].getType(), "test_comment_prefix_adjacent_to_word_is_swallowed", LINE_DATA());
	return errors;
}

// ── addQuote() — caminho feliz ────────────────────────────────────────────────

int test_quote_basic()
{
	write_file("/tmp/lexer_t19.txt", "x = \"hello world\"");
	Lexer lex("/tmp/lexer_t19.txt");
	lex.addDelimitter("=", LexerTokenType::DELIM_ASSIGN);
	lex.addQuote("\"", LexerTokenType::STRING_DOUBLE);
	lex.run();
	const std::vector<token> &t = lex.tokens();
	int errors = 0;
	errors += assert_true(t.size() >= 3, "test_quote_basic", LINE_DATA());
	errors += assert(std::string("x"), t[0].getContent(), "test_quote_basic", LINE_DATA());
	errors += assert(LexerTokenType::DELIM_ASSIGN, t[1].getType(), "test_quote_basic", LINE_DATA());
	errors += assert(std::string("="), t[1].getContent(), "test_quote_basic", LINE_DATA());
	errors += assert(LexerTokenType::STRING_DOUBLE, t[2].getType(), "test_quote_basic", LINE_DATA());
	errors += assert(std::string("hello world"), t[2].getContent(), "test_quote_basic", LINE_DATA());
	return errors;
}

// Feature: a string capturada não inclui os delimitadores de abertura/fechamento
int test_quote_text_excludes_delimiters()
{
	write_file("/tmp/lexer_t20.txt", "\"abc\"");
	Lexer lex("/tmp/lexer_t20.txt");
	lex.addQuote("\"", LexerTokenType::STRING_DOUBLE);
	lex.run();
	const std::vector<token> &t = lex.tokens();
	bool ok = !t.empty() && t[0].getContent() == "abc";
	return assert_true(ok, "test_quote_text_excludes_delimiters", LINE_DATA());
}

// Feature: delimitador de quote multi-caractere (ex.: ''')
int test_quote_multi_char_delimiter()
{
	write_file("/tmp/lexer_t21.txt", "'''hello'''");
	Lexer lex("/tmp/lexer_t21.txt");
	lex.addQuote("'''", LexerTokenType::STRING_SINGLE);
	lex.run();
	const std::vector<token> &t = lex.tokens();
	int errors = 0;
	errors += assert_true(!t.empty(), "test_quote_multi_char_delimiter", LINE_DATA());
	if (errors) return errors;
	errors += assert(LexerTokenType::STRING_SINGLE, t[0].getType(), "test_quote_multi_char_delimiter", LINE_DATA());
	errors += assert(std::string("hello"), t[0].getContent(), "test_quote_multi_char_delimiter", LINE_DATA());
	return errors;
}

// Feature: caractere de escape '\' faz o caractere seguinte ser pulado, então
// uma aspa escapada ("\\\"") não fecha a string prematuramente
int test_quote_escaped_quote_does_not_close()
{
	// conteúdo no arquivo: "hello \"world\""
	write_file("/tmp/lexer_t22.txt", "\"hello \\\"world\\\"\"");
	Lexer lex("/tmp/lexer_t22.txt");
	lex.addQuote("\"", LexerTokenType::STRING_DOUBLE);
	lex.run();
	const std::vector<token> &t = lex.tokens();
	bool ok = !t.empty() && t[0].getType() == LexerTokenType::STRING_DOUBLE
		&& t[0].getContent() == "hello \\\"world\\\"";
	return assert_true(ok, "test_quote_escaped_quote_does_not_close", LINE_DATA());
}

// Feature: múltiplos delimitadores de quote distintos (ex.: " e ')
int test_quote_multiple_distinct_delimiters()
{
	write_file("/tmp/lexer_t23.txt", "\"double\" 'single'");
	Lexer lex("/tmp/lexer_t23.txt");
	lex.addQuote("\"", LexerTokenType::STRING_DOUBLE);
	lex.addQuote("'", LexerTokenType::STRING_SINGLE);
	lex.run();
	const std::vector<token> &t = lex.tokens();
	bool ok = t.size() >= 2
		&& t[0].getType() == LexerTokenType::STRING_DOUBLE && t[0].getContent() == "double"
		&& t[1].getType() == LexerTokenType::STRING_SINGLE && t[1].getContent() == "single";
	return assert_true(ok, "test_quote_multiple_distinct_delimiters", LINE_DATA());
}

// Feature: string vazia ("") é capturada corretamente
int test_quote_empty_content()
{
	write_file("/tmp/lexer_t24.txt", "\"\"");
	Lexer lex("/tmp/lexer_t24.txt");
	lex.addQuote("\"", LexerTokenType::STRING_DOUBLE);
	lex.run();
	const std::vector<token> &t = lex.tokens();
	bool ok = !t.empty() && t[0].getType() == LexerTokenType::STRING_DOUBLE && t[0].getContent().empty();
	return assert_true(ok, "test_quote_empty_content", LINE_DATA());
}

// ── addQuote() — strings não fechadas (erro) ──────────────────────────────────

int test_quote_unterminated_sets_error()
{
	write_file("/tmp/lexer_t25.txt", "x = \"unterminated");
	Lexer lex("/tmp/lexer_t25.txt");
	lex.addDelimitter("=", LexerTokenType::DELIM_ASSIGN);
	lex.addQuote("\"", LexerTokenType::STRING_DOUBLE);
	lex.run();
	bool ok = !lex.error().empty()
		&& lex.error().find("quote open") != std::string::npos;
	return assert_true(ok, "test_quote_unterminated_sets_error", LINE_DATA());
}

int test_quote_unterminated_emits_error_token()
{
	write_file("/tmp/lexer_t26.txt", "\"unterminated");
	Lexer lex("/tmp/lexer_t26.txt");
	lex.addQuote("\"", LexerTokenType::STRING_DOUBLE);
	lex.run();
	const std::vector<token> &t = lex.tokens();
	bool ok = !t.empty() && t.back().getType() == LexerTokenType::ERROR;
	return assert_true(ok, "test_quote_unterminated_emits_error_token", LINE_DATA());
}

// Feature: a mensagem de erro reporta a posição de ABERTURA da quote, não a
// posição onde o arquivo terminou
int test_quote_unterminated_error_reports_open_position()
{
	write_file("/tmp/lexer_t27.txt", "x = \"unterminated");
	Lexer lex("/tmp/lexer_t27.txt");
	lex.addDelimitter("=", LexerTokenType::DELIM_ASSIGN);
	lex.addQuote("\"", LexerTokenType::STRING_DOUBLE);
	lex.run();
	// a quote abre na coluna 5 (0-based: 'x'=0,' '=1,'='=2,' '=3,'"'=4, depois
	// do += do delimitador de abertura, lineColumn() é a coluna seguinte)
	bool ok = lex.error().find(":1:5") != std::string::npos;
	return assert_true(ok, "test_quote_unterminated_error_reports_open_position", LINE_DATA());
}

// Feature: quote não terminada que se estende por múltiplas linhas é
// consumida inteiramente (o laço interno de _readQuote ignora '\n')
int test_quote_unterminated_spans_multiple_lines()
{
	write_file("/tmp/lexer_t28.txt", "\"line1\nline2 unterminated");
	Lexer lex("/tmp/lexer_t28.txt");
	lex.addQuote("\"", LexerTokenType::STRING_DOUBLE);
	lex.run();
	bool ok = !lex.error().empty();
	return assert_true(ok, "test_quote_unterminated_spans_multiple_lines", LINE_DATA());
}

// ── Interação entre comentário, delimitador e quote ──────────────────────────

// Feature: quando um prefixo é registrado tanto como delimitador quanto como
// comentário, a checagem de comentário (que ocorre primeiro no laço de run())
// tem prioridade
int test_comment_takes_priority_over_delimitter_with_same_prefix()
{
	write_file("/tmp/lexer_t29.txt", "a#b");
	Lexer lex("/tmp/lexer_t29.txt");
	lex.addDelimitter("#", LexerTokenType::DELIM_PLUS);
	lex.addComment("#");
	lex.run();
	const std::vector<token> &t = lex.tokens();
	bool ok = t.size() >= 2
		&& t[0].getContent() == "a"
		&& t[1].getType() == LexerTokenType::COMMENT; // não DELIM_PLUS
	return assert_true(ok, "test_comment_takes_priority_over_delimitter_with_same_prefix", LINE_DATA());
}

// Feature: dentro de uma word, um delimitador interrompe a leitura mesmo sem
// espaço antes dele (o laço de _readWord verifica _isDelimitter() a cada
// caractere)
int test_word_stops_at_delimitter_without_space()
{
	write_file("/tmp/lexer_t30.txt", "key=value");
	Lexer lex("/tmp/lexer_t30.txt");
	lex.addDelimitter("=", LexerTokenType::DELIM_ASSIGN);
	lex.run();
	const std::vector<token> &t = lex.tokens();
	bool ok = t.size() >= 3
		&& t[0].getContent() == "key" && t[1].getContent() == "=" && t[2].getContent() == "value";
	return assert_true(ok, "test_word_stops_at_delimitter_without_space", LINE_DATA());
}

// Feature: dentro de uma word, uma abertura de quote interrompe a leitura
// mesmo sem espaço antes dela
int test_word_stops_at_quote_without_space()
{
	write_file("/tmp/lexer_t31.txt", "key\"value\"");
	Lexer lex("/tmp/lexer_t31.txt");
	lex.addQuote("\"", LexerTokenType::STRING_DOUBLE);
	lex.run();
	const std::vector<token> &t = lex.tokens();
	bool ok = t.size() >= 2
		&& t[0].getContent() == "key" && t[0].getType() == LexerTokenType::WORD
		&& t[1].getContent() == "value" && t[1].getType() == LexerTokenType::STRING_DOUBLE;
	return assert_true(ok, "test_word_stops_at_quote_without_space", LINE_DATA());
}

// ── error() / tokens() — acessores ────────────────────────────────────────────

int test_error_empty_when_no_problem()
{
	Lexer lex = run_on("/tmp/lexer_t32.txt", "all good here");
	return assert_true(lex.error().empty(), "test_error_empty_when_no_problem", LINE_DATA());
}

int test_tokens_accessor_returns_same_count_as_processed()
{
	Lexer lex = run_on("/tmp/lexer_t33.txt", "a b c");
	// 3 words + 1 END = 4
	return assert_true(lex.tokens().size() == 4, "test_tokens_accessor_returns_same_count_as_processed", LINE_DATA());
}

// ── rastreamento de linha/coluna nos tokens ───────────────────────────────────

// Feature: o token de uma word na primeira linha reporta line=1
int test_token_line_tracking_first_line()
{
	Lexer lex = run_on("/tmp/lexer_t34.txt", "hello");
	bool ok = !lex.tokens().empty() && lex.tokens()[0].getLine() == 1;
	return assert_true(ok, "test_token_line_tracking_first_line", LINE_DATA());
}

// Feature: um token após uma quebra de linha reporta line=2
int test_token_line_tracking_second_line()
{
	Lexer lex = run_on("/tmp/lexer_t35.txt", "foo\nbar");
	const std::vector<token> &t = lex.tokens();
	bool ok = t.size() >= 2 && t[0].getLine() == 1 && t[1].getLine() == 2;
	return assert_true(ok, "test_token_line_tracking_second_line", LINE_DATA());
}

// ── main ──────────────────────────────────────────────────────────────────────

int main(int argc, char **argv)
{
	messages::settup(argc, argv);

	int failures = 0;

	// std::cout << "\n=== Construtor / estado inicial ===\n";
	failures += test_ctor_default_no_tokens();
	failures += test_ctor_default_no_error_before_run();
	failures += test_ctor_with_filename_no_tokens_before_run();

	// std::cout << "\n=== run() — palavras simples ===\n";
	failures += test_run_single_word();
	failures += test_run_multiple_words_separated_by_space();
	failures += test_run_ends_with_end_token();
	failures += test_run_only_whitespace_yields_only_end();
	failures += test_run_multiple_whitespace_between_words();

	// std::cout << "\n=== run() — arquivo vazio / inexistente ===\n";
	failures += test_run_empty_file_no_tokens();
	failures += test_run_empty_file_no_error();
	failures += test_run_missing_file_sets_error();
	failures += test_run_missing_file_no_tokens();

	// std::cout << "\n=== setFile() ===\n";
	failures += test_setFile_switches_target();

	// std::cout << "\n=== addDelimitter() ===\n";
	failures += test_delimitter_single_char();
	failures += test_delimitter_multi_char();
	failures += test_delimitter_adjacent_to_word_no_space();
	failures += test_delimitter_multiple_distinct();

	// std::cout << "\n=== BUG: delimitador vazio causa avanço zero (loop infinito) ===\n";
	failures += test_delimitter_empty_string_is_false();

	// std::cout << "\n=== addComment() ===\n";
	failures += test_comment_basic();
	failures += test_comment_captures_prefix_to_text();
	failures += test_comment_at_end_of_file_without_trailing_newline();

	// std::cout << "\n=== BUG: comentário inclui '\\n' final ===\n";
	failures += test_comment_dont_includes_trailing_newline();

	// std::cout << "\n=== BUG: _readWord não verifica _isComment ===\n";
	failures += test_comment_prefix_adjacent_to_word_is_swallowed();

	// std::cout << "\n=== addQuote() — caminho feliz ===\n";
	failures += test_quote_basic();
	failures += test_quote_text_excludes_delimiters();
	failures += test_quote_multi_char_delimiter();
	failures += test_quote_escaped_quote_does_not_close();
	failures += test_quote_multiple_distinct_delimiters();
	failures += test_quote_empty_content();

	// std::cout << "\n=== addQuote() — strings não fechadas (erro) ===\n";
	failures += test_quote_unterminated_sets_error();
	failures += test_quote_unterminated_emits_error_token();
	failures += test_quote_unterminated_error_reports_open_position();
	failures += test_quote_unterminated_spans_multiple_lines();

	// std::cout << "\n=== Interação entre comentário, delimitador e quote ===\n";
	failures += test_comment_takes_priority_over_delimitter_with_same_prefix();
	failures += test_word_stops_at_delimitter_without_space();
	failures += test_word_stops_at_quote_without_space();

	// std::cout << "\n=== error() / tokens() ===\n";
	failures += test_error_empty_when_no_problem();
	failures += test_tokens_accessor_returns_same_count_as_processed();

	// std::cout << "\n=== rastreamento de linha/coluna ===\n";
	failures += test_token_line_tracking_first_line();
	failures += test_token_line_tracking_second_line();

	messages::print();

	return failures;
}
