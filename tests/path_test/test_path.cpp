/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_path.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/04 10:16:15 by ighannam          #+#    #+#             */
/*   Updated: 2026/07/04 10:35:41 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include <string>

#include "Path.hpp"
#include "test_utils.hpp"

// ── normalizePath / getNormalizedPath (paths absolutos) ────────────────────

int test_normalize_simple_absolute_path()
{
	Path p("/foo/bar");
	return assert_true(p.getNormalizedPath() == "/foo/bar",
		"test_normalize_simple_absolute_path", LINE_DATA());
}

int test_normalize_root_path()
{
	Path p("/");
	return assert_true(p.getNormalizedPath() == "/",
		"test_normalize_root_path", LINE_DATA());
}

int test_normalize_removes_dot_segments()
{
	Path p("/foo/./bar");
	return assert_true(p.getNormalizedPath() == "/foo/bar",
		"test_normalize_removes_dot_segments", LINE_DATA());
}

int test_normalize_resolves_dotdot_segments()
{
	Path p("/foo/bar/../baz");
	return assert_true(p.getNormalizedPath() == "/foo/baz",
		"test_normalize_resolves_dotdot_segments", LINE_DATA());
}

int test_normalize_collapses_double_slashes()
{
	Path p("//foo//bar");
	return assert_true(p.getNormalizedPath() == "/foo/bar",
		"test_normalize_collapses_double_slashes", LINE_DATA());
}

int test_normalize_strips_trailing_slash()
{
	Path p("/foo/bar/");
	return assert_true(p.getNormalizedPath() == "/foo/bar",
		"test_normalize_strips_trailing_slash", LINE_DATA());
}

int test_normalize_rejects_traversal_above_root()
{
	Path p("/../foo");
	return assert_true(p.getNormalizedPath().empty(),
		"test_normalize_rejects_traversal_above_root", LINE_DATA());
}

int test_normalize_rejects_multiple_traversal_above_root()
{
	Path p("/foo/../..");
	return assert_true(p.getNormalizedPath().empty(),
		"test_normalize_rejects_multiple_traversal_above_root", LINE_DATA());
}

int test_normalize_empty_input_stays_empty()
{
	Path p("");
	return assert_true(p.getNormalizedPath().empty(),
		"test_normalize_empty_input_stays_empty", LINE_DATA());
}

// ── normalizePath (paths relativos, com is_absolute) ────────────────────────

int test_normalize_relative_path_stays_relative()
{
	Path p("foo/bar");
	return assert_true(p.getNormalizedPath() == "foo/bar",
		"test_normalize_relative_path_stays_relative", LINE_DATA());
}

int test_normalize_relative_dot_segment_removed()
{
	Path p("foo/./bar");
	return assert_true(p.getNormalizedPath() == "foo/bar",
		"test_normalize_relative_dot_segment_removed", LINE_DATA());
}

int test_normalize_relative_dotdot_resolves()
{
	Path p("foo/bar/../baz");
	return assert_true(p.getNormalizedPath() == "foo/baz",
		"test_normalize_relative_dotdot_resolves", LINE_DATA());
}

// Feature (QUIRK documentado, não necessariamente desejado): um path relativo
// que se resolve inteiramente (ex: "a/..") acaba virando o path absoluto "/",
// misturando o conceito de "diretório atual relativo" com "raiz absoluta".
// Este teste existe para deixar esse comportamento visível caso a equipe
// decida que ele precisa mudar.
int test_normalize_relative_fully_resolved_becomes_absolute_root_quirk()
{
	Path p("a/..");
	return assert_true(p.getNormalizedPath() == "/",
		"test_normalize_relative_fully_resolved_becomes_absolute_root_quirk", LINE_DATA());
}

// ── isNormalizable ────────────────────────────────────────────────────────

int test_is_normalizable_valid_path_returns_true()
{
	Path p("/foo/bar");
	return assert_true(p.isNormalizable(),
		"test_is_normalizable_valid_path_returns_true", LINE_DATA());
}

int test_is_normalizable_traversal_above_root_returns_false()
{
	Path p("/../foo");
	return assert_true(!p.isNormalizable(),
		"test_is_normalizable_traversal_above_root_returns_false", LINE_DATA());
}

int test_is_normalizable_empty_input_returns_false()
{
	Path p("");
	return assert_true(!p.isNormalizable(),
		"test_is_normalizable_empty_input_returns_false", LINE_DATA());
}

// ── getPath (input bruto, não normalizado) ─────────────────────────────────

int test_get_path_returns_original_unmodified_input()
{
	Path p("/foo/../bar");
	return assert_true(p.getPath() == "/foo/../bar",
		"test_get_path_returns_original_unmodified_input", LINE_DATA());
}

// ── query string / clean path ──────────────────────────────────────────────

int test_query_string_extracted_after_question_mark()
{
	Path p("/foo/bar?x=1");
	return assert_true(p.getQueryString() == "x=1",
		"test_query_string_extracted_after_question_mark", LINE_DATA());
}

int test_query_string_empty_when_no_question_mark()
{
	Path p("/foo/bar");
	return assert_true(p.getQueryString().empty(),
		"test_query_string_empty_when_no_question_mark", LINE_DATA());
}

int test_clean_path_strips_query_string()
{
	Path p("/foo/bar?x=1");
	return assert_true(p.getCleanPath() == "/foo/bar",
		"test_clean_path_strips_query_string", LINE_DATA());
}

int test_clean_path_equals_normalized_when_no_query()
{
	Path p("/foo/bar");
	return assert_true(p.getCleanPath() == p.getNormalizedPath(),
		"test_clean_path_equals_normalized_when_no_query", LINE_DATA());
}

// ── getFilename ─────────────────────────────────────────────────────────────

int test_filename_simple_case()
{
	Path p("/foo/bar.html");
	return assert_true(p.getFilename() == "bar.html",
		"test_filename_simple_case", LINE_DATA());
}

int test_filename_excludes_leading_slash()
{
	Path p("/foo/bar.html");
	return assert_true(p.getFilename()[0] != '/',
		"test_filename_excludes_leading_slash", LINE_DATA());
}

int test_filename_no_extension()
{
	Path p("/foo/bar");
	return assert_true(p.getFilename() == "bar",
		"test_filename_no_extension", LINE_DATA());
}

int test_filename_root_path_is_empty()
{
	Path p("/");
	return assert_true(p.getFilename().empty(),
		"test_filename_root_path_is_empty", LINE_DATA());
}

int test_filename_dot_in_directory_not_treated_as_file_dot()
{
	Path p("/foo.bar/baz");
	return assert_true(p.getFilename() == "baz",
		"test_filename_dot_in_directory_not_treated_as_file_dot", LINE_DATA());
}

int test_filename_not_corrupted_by_query_string()
{
	Path p("/foo/bar?file=a.txt");
	return assert_true(p.getFilename() == "bar",
		"test_filename_not_corrupted_by_query_string", LINE_DATA());
}

int test_filename_not_corrupted_by_slash_in_query_string()
{
	Path p("/foo/bar?redirect=/home");
	return assert_true(p.getFilename() == "bar",
		"test_filename_not_corrupted_by_slash_in_query_string", LINE_DATA());
}

int test_filename_relative_without_slash_with_extension()
{
	Path p("bar.txt");
	return assert_true(p.getFilename() == "bar.txt",
		"test_filename_relative_without_slash_with_extension", LINE_DATA());
}

int test_filename_relative_without_slash_no_extension()
{
	Path p("bar");
	return assert_true(p.getFilename() == "bar",
		"test_filename_relative_without_slash_no_extension", LINE_DATA());
}

// ── getExtension ────────────────────────────────────────────────────────────

int test_extension_simple_case()
{
	Path p("/foo/bar.html");
	return assert_true(p.getExtension() == ".html",
		"test_extension_simple_case", LINE_DATA());
}

int test_extension_empty_when_no_dot()
{
	Path p("/foo/bar");
	return assert_true(p.getExtension().empty(),
		"test_extension_empty_when_no_dot", LINE_DATA());
}

int test_extension_ignores_dot_in_directory_name()
{
	Path p("/foo.bar/baz");
	return assert_true(p.getExtension().empty(),
		"test_extension_ignores_dot_in_directory_name", LINE_DATA());
}

int test_extension_not_leaked_from_query_string()
{
	Path p("/foo/bar?file=a.txt");
	return assert_true(p.getExtension().empty(),
		"test_extension_not_leaked_from_query_string", LINE_DATA());
}

int test_extension_correct_alongside_query_string()
{
	Path p("/foo/bar.js?v=2");
	return assert_true(p.getExtension() == ".js",
		"test_extension_correct_alongside_query_string", LINE_DATA());
}

int test_extension_relative_without_slash()
{
	Path p("bar.txt");
	return assert_true(p.getExtension() == ".txt",
		"test_extension_relative_without_slash", LINE_DATA());
}

int test_extension_relative_without_slash_no_dot()
{
	Path p("bar");
	return assert_true(p.getExtension().empty(),
		"test_extension_relative_without_slash_no_dot", LINE_DATA());
}

// Feature (QUIRK documentado, decisão consciente): dotfiles não são tratados
// como caso especial neste projeto. Um "." na posição 0 (sem barra) ainda é
// lido como separador de extensão, então o dotfile inteiro vira "extensão"
// e o basename correspondente fica vazio. Ver test_basename_dotfile_without_slash_quirk.
int test_extension_dotfile_without_slash_quirk()
{
	Path p(".htaccess");
	return assert_true(p.getExtension() == ".htaccess",
		"test_extension_dotfile_without_slash_quirk", LINE_DATA());
}

// ── getBasename ─────────────────────────────────────────────────────────────

int test_basename_simple_case()
{
	Path p("/foo/bar.html");
	return assert_true(p.getBasename() == "bar",
		"test_basename_simple_case", LINE_DATA());
}

int test_basename_equals_filename_when_no_extension()
{
	Path p("/foo/bar");
	return assert_true(p.getBasename() == "bar",
		"test_basename_equals_filename_when_no_extension", LINE_DATA());
}

int test_basename_root_path_is_empty()
{
	Path p("/");
	return assert_true(p.getBasename().empty(),
		"test_basename_root_path_is_empty", LINE_DATA());
}

int test_basename_ignores_dot_in_directory_name()
{
	Path p("/foo.bar/baz");
	return assert_true(p.getBasename() == "baz",
		"test_basename_ignores_dot_in_directory_name", LINE_DATA());
}

int test_basename_relative_without_slash_with_extension()
{
	Path p("bar.txt");
	return assert_true(p.getBasename() == "bar",
		"test_basename_relative_without_slash_with_extension", LINE_DATA());
}

int test_basename_relative_without_slash_no_extension()
{
	Path p("bar");
	return assert_true(p.getBasename() == "bar",
		"test_basename_relative_without_slash_no_extension", LINE_DATA());
}

int test_basename_dotfile_without_slash_quirk()
{
	Path p(".htaccess");
	return assert_true(p.getBasename().empty(),
		"test_basename_dotfile_without_slash_quirk", LINE_DATA());
}

// ── getLastDir ──────────────────────────────────────────────────────────────

int test_last_dir_simple_case()
{
	Path p("/foo/bar.html");
	return assert_true(p.getLastDir() == "/foo/",
		"test_last_dir_simple_case", LINE_DATA());
}

int test_last_dir_nested()
{
	Path p("/a/b/c.txt");
	return assert_true(p.getLastDir() == "/a/b/",
		"test_last_dir_nested", LINE_DATA());
}

int test_last_dir_root_path()
{
	Path p("/");
	return assert_true(p.getLastDir() == "/",
		"test_last_dir_root_path", LINE_DATA());
}

int test_last_dir_ignores_dot_in_directory_name()
{
	Path p("/foo.bar/baz");
	return assert_true(p.getLastDir() == "/foo.bar/",
		"test_last_dir_ignores_dot_in_directory_name", LINE_DATA());
}

int test_last_dir_relative_without_slash_stays_empty()
{
	Path p("bar.txt");
	return assert_true(p.getLastDir().empty(),
		"test_last_dir_relative_without_slash_stays_empty", LINE_DATA());
}

// ── operator+ ───────────────────────────────────────────────────────────────

int test_concat_no_slashes_adds_separator()
{
	Path base("/foo");
	Path rel("bar");
	Path result = base + rel;
	return assert_true(result.getNormalizedPath() == "/foo/bar",
		"test_concat_no_slashes_adds_separator", LINE_DATA());
}

int test_concat_right_leading_slash_no_double_slash()
{
	Path base("/foo");
	Path rel("/bar");
	Path result = base + rel;
	return assert_true(result.getNormalizedPath() == "/foo/bar",
		"test_concat_right_leading_slash_no_double_slash", LINE_DATA());
}

int test_concat_left_root_no_double_slash()
{
	Path base("/");
	Path rel("/bar");
	Path result = base + rel;
	return assert_true(result.getNormalizedPath() == "/bar",
		"test_concat_left_root_no_double_slash", LINE_DATA());
}

int test_concat_left_clean_path_trailing_slash_no_double_slash()
{
	Path base("/foo/?x=1");
	Path rel("/bar");
	Path result = base + rel;
	return assert_true(result.getNormalizedPath() == "/foo/bar",
		"test_concat_left_clean_path_trailing_slash_no_double_slash", LINE_DATA());
}

int test_concat_invalid_left_operand_yields_invalid_result()
{
	Path invalid("/../foo");
	Path valid("/bar");
	Path result = invalid + valid;
	return assert_true(!result.isNormalizable(),
		"test_concat_invalid_left_operand_yields_invalid_result", LINE_DATA());
}

int test_concat_invalid_right_operand_yields_invalid_result()
{
	Path valid("/foo");
	Path invalid("/../bar");
	Path result = valid + invalid;
	return assert_true(!result.isNormalizable(),
		"test_concat_invalid_right_operand_yields_invalid_result", LINE_DATA());
}

// ── operator<< ────────────────────────────────────────────────────────────

int test_print_contains_normalized_path()
{
	Path p("/foo/bar.html");
	std::ostringstream oss;
	oss << p;
	return assert_true(oss.str().find("/foo/bar.html") != std::string::npos,
		"test_print_contains_normalized_path", LINE_DATA());
}

int test_print_contains_filename()
{
	Path p("/foo/bar.html");
	std::ostringstream oss;
	oss << p;
	return assert_true(oss.str().find("filename=bar.html") != std::string::npos,
		"test_print_contains_filename", LINE_DATA());
}

int test_print_contains_query_string()
{
	Path p("/foo/bar?x=1");
	std::ostringstream oss;
	oss << p;
	return assert_true(oss.str().find("query string=x=1") != std::string::npos,
		"test_print_contains_query_string", LINE_DATA());
}

// ── main ──────────────────────────────────────────────────────────────────────

int main(int argc, char **argv)
{
	messages::settup(argc, argv);

	int failures = 0;

	failures += test_normalize_simple_absolute_path();
	failures += test_normalize_root_path();
	failures += test_normalize_removes_dot_segments();
	failures += test_normalize_resolves_dotdot_segments();
	failures += test_normalize_collapses_double_slashes();
	failures += test_normalize_strips_trailing_slash();
	failures += test_normalize_rejects_traversal_above_root();
	failures += test_normalize_rejects_multiple_traversal_above_root();
	failures += test_normalize_empty_input_stays_empty();

	failures += test_normalize_relative_path_stays_relative();
	failures += test_normalize_relative_dot_segment_removed();
	failures += test_normalize_relative_dotdot_resolves();
	failures += test_normalize_relative_fully_resolved_becomes_absolute_root_quirk();

	failures += test_is_normalizable_valid_path_returns_true();
	failures += test_is_normalizable_traversal_above_root_returns_false();
	failures += test_is_normalizable_empty_input_returns_false();

	failures += test_get_path_returns_original_unmodified_input();

	failures += test_query_string_extracted_after_question_mark();
	failures += test_query_string_empty_when_no_question_mark();
	failures += test_clean_path_strips_query_string();
	failures += test_clean_path_equals_normalized_when_no_query();

	failures += test_filename_simple_case();
	failures += test_filename_excludes_leading_slash();
	failures += test_filename_no_extension();
	failures += test_filename_root_path_is_empty();
	failures += test_filename_dot_in_directory_not_treated_as_file_dot();
	failures += test_filename_not_corrupted_by_query_string();
	failures += test_filename_not_corrupted_by_slash_in_query_string();
	failures += test_filename_relative_without_slash_with_extension();
	failures += test_filename_relative_without_slash_no_extension();

	failures += test_extension_simple_case();
	failures += test_extension_empty_when_no_dot();
	failures += test_extension_ignores_dot_in_directory_name();
	failures += test_extension_not_leaked_from_query_string();
	failures += test_extension_correct_alongside_query_string();
	failures += test_extension_relative_without_slash();
	failures += test_extension_relative_without_slash_no_dot();
	failures += test_extension_dotfile_without_slash_quirk();

	failures += test_basename_simple_case();
	failures += test_basename_equals_filename_when_no_extension();
	failures += test_basename_root_path_is_empty();
	failures += test_basename_ignores_dot_in_directory_name();
	failures += test_basename_relative_without_slash_with_extension();
	failures += test_basename_relative_without_slash_no_extension();
	failures += test_basename_dotfile_without_slash_quirk();

	failures += test_last_dir_simple_case();
	failures += test_last_dir_nested();
	failures += test_last_dir_root_path();
	failures += test_last_dir_ignores_dot_in_directory_name();
	failures += test_last_dir_relative_without_slash_stays_empty();

	failures += test_concat_no_slashes_adds_separator();
	failures += test_concat_right_leading_slash_no_double_slash();
	failures += test_concat_left_root_no_double_slash();
	failures += test_concat_left_clean_path_trailing_slash_no_double_slash();
	failures += test_concat_invalid_left_operand_yields_invalid_result();
	failures += test_concat_invalid_right_operand_yields_invalid_result();

	failures += test_print_contains_normalized_path();
	failures += test_print_contains_filename();
	failures += test_print_contains_query_string();

	messages::print();

	return failures;
}