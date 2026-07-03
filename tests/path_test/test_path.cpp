/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_path.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 10:12:15 by ighannam          #+#    #+#             */
/*   Updated: 2026/07/03 11:30:03 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "path.hpp"
#include "test_utils.hpp"

int test_normalize_simple_absolute_path()
{
	path p("/foo/bar");
	return assert_true(p.getNormalizedPath() == "/foo/bar",
		"test_normalize_simple_absolute_path", LINE_DATA());
}

int test_normalize_root_path()
{
	path p("/");
	return assert_true(p.getNormalizedPath() == "/",
		"test_normalize_root_path", LINE_DATA());
}

int test_normalize_removes_dot_segments()
{
	path p("/foo/./bar");
	return assert_true(p.getNormalizedPath() == "/foo/bar",
		"test_normalize_removes_dot_segments", LINE_DATA());
}

int test_normalize_resolves_dotdot_segments()
{
	path p("/foo/bar/../baz");
	return assert_true(p.getNormalizedPath() == "/foo/baz",
		"test_normalize_resolves_dotdot_segments", LINE_DATA());
}

int test_normalize_collapses_double_slashes()
{
	path p("//foo//bar");
	return assert_true(p.getNormalizedPath() == "/foo/bar",
		"test_normalize_collapses_double_slashes", LINE_DATA());
}

int test_normalize_strips_trailing_slash()
{
	path p("/foo/bar/");
	return assert_true(p.getNormalizedPath() == "/foo/bar",
		"test_normalize_strips_trailing_slash", LINE_DATA());
}

int test_normalize_rejects_traversal_above_root()
{
	path p("/../foo");
	return assert_true(p.getNormalizedPath().empty(),
		"test_normalize_rejects_traversal_above_root", LINE_DATA());
}

int test_normalize_rejects_multiple_traversal_above_root()
{
	path p("/foo/../..");
	return assert_true(p.getNormalizedPath().empty(),
		"test_normalize_rejects_multiple_traversal_above_root", LINE_DATA());
}

int test_normalize_empty_input_stays_empty()
{
	path p("");
	return assert_true(p.getNormalizedPath().empty(),
		"test_normalize_empty_input_stays_empty", LINE_DATA());
}

int test_normalize_relative_path_gets_leading_slash()
{
	path p("foo/bar");
	return assert_true(p.getNormalizedPath() == "/foo/bar",
		"test_normalize_relative_path_gets_leading_slash", LINE_DATA());
}

int test_validate_path_valid_returns_zero()
{
	path p("/foo/bar");
	return assert(0, p.validatePath(),
		"test_validate_path_valid_returns_zero", LINE_DATA());
}

int test_validate_path_traversal_returns_403()
{
	path p("/../foo");
	return assert(403, p.validatePath(),
		"test_validate_path_traversal_returns_403", LINE_DATA());
}

int test_validate_path_empty_input_returns_403()
{
	path p("");
	return assert(403, p.validatePath(),
		"test_validate_path_empty_input_returns_403", LINE_DATA());
}

int test_get_path_returns_original_unmodified_input()
{
	path p("/foo/../bar");
	return assert_true(p.getPath() == "/foo/../bar",
		"test_get_path_returns_original_unmodified_input", LINE_DATA());
}

int test_query_string_extracted_after_question_mark()
{
	path p("/foo/bar?x=1");
	return assert_true(p.getQueryString() == "x=1",
		"test_query_string_extracted_after_question_mark", LINE_DATA());
}

int test_query_string_empty_when_no_question_mark()
{
	path p("/foo/bar");
	return assert_true(p.getQueryString().empty(),
		"test_query_string_empty_when_no_question_mark", LINE_DATA());
}

int test_clean_path_strips_query_string()
{
	path p("/foo/bar?x=1");
	return assert_true(p.getCleanPath() == "/foo/bar",
		"test_clean_path_strips_query_string", LINE_DATA());
}

int test_clean_path_equals_normalized_when_no_query()
{
	path p("/foo/bar");
	return assert_true(p.getCleanPath() == p.getNormalizedPath(),
		"test_clean_path_equals_normalized_when_no_query", LINE_DATA());
}

int test_filename_simple_case()
{
	path p("/foo/bar.html");
	return assert_true(p.getFilename() == "bar.html",
		"test_filename_simple_case", LINE_DATA());
}

int test_filename_excludes_leading_slash()
{
	path p("/foo/bar.html");
	return assert_true(p.getFilename()[0] != '/',
		"test_filename_excludes_leading_slash", LINE_DATA());
}

int test_filename_no_extension()
{
	path p("/foo/bar");
	return assert_true(p.getFilename() == "bar",
		"test_filename_no_extension", LINE_DATA());
}

int test_filename_root_path_is_empty()
{
	path p("/");
	return assert_true(p.getFilename().empty(),
		"test_filename_root_path_is_empty", LINE_DATA());
}

int test_filename_dot_in_directory_not_treated_as_file_dot()
{
	path p("/foo.bar/baz");
	return assert_true(p.getFilename() == "baz",
		"test_filename_dot_in_directory_not_treated_as_file_dot", LINE_DATA());
}

int test_filename_not_corrupted_by_query_string()
{
	path p("/foo/bar?file=a.txt");
	return assert_true(p.getFilename() == "bar",
		"test_filename_not_corrupted_by_query_string", LINE_DATA());
}

int test_filename_not_corrupted_by_slash_in_query_string()
{
	path p("/foo/bar?redirect=/home");
	return assert_true(p.getFilename() == "bar",
		"test_filename_not_corrupted_by_slash_in_query_string", LINE_DATA());
}

int test_extension_simple_case()
{
	path p("/foo/bar.html");
	return assert_true(p.getExtension() == ".html",
		"test_extension_simple_case", LINE_DATA());
}

int test_extension_empty_when_no_dot()
{
	path p("/foo/bar");
	return assert_true(p.getExtension().empty(),
		"test_extension_empty_when_no_dot", LINE_DATA());
}

int test_extension_ignores_dot_in_directory_name()
{
	path p("/foo.bar/baz");
	return assert_true(p.getExtension().empty(),
		"test_extension_ignores_dot_in_directory_name", LINE_DATA());
}

int test_extension_not_leaked_from_query_string()
{
	path p("/foo/bar?file=a.txt");
	return assert_true(p.getExtension().empty(),
		"test_extension_not_leaked_from_query_string", LINE_DATA());
}

int test_extension_correct_alongside_query_string()
{
	path p("/foo/bar.js?v=2");
	return assert_true(p.getExtension() == ".js",
		"test_extension_correct_alongside_query_string", LINE_DATA());
}

int test_basename_simple_case()
{
	path p("/foo/bar.html");
	return assert_true(p.getBasename() == "bar",
		"test_basename_simple_case", LINE_DATA());
}

int test_basename_equals_filename_when_no_extension()
{
	path p("/foo/bar");
	return assert_true(p.getBasename() == "bar",
		"test_basename_equals_filename_when_no_extension", LINE_DATA());
}

int test_basename_root_path_is_empty()
{
	path p("/");
	return assert_true(p.getBasename().empty(),
		"test_basename_root_path_is_empty", LINE_DATA());
}

int test_basename_ignores_dot_in_directory_name()
{
	path p("/foo.bar/baz");
	return assert_true(p.getBasename() == "baz",
		"test_basename_ignores_dot_in_directory_name", LINE_DATA());
}

int test_concat_no_slashes_adds_separator()
{
	path base("/foo");
	path rel("bar");
	path result = base + rel;
	return assert_true(result.getNormalizedPath() == "/foo/bar",
		"test_concat_no_slashes_adds_separator", LINE_DATA());
}

int test_concat_right_leading_slash_no_double_slash()
{
	path base("/foo");
	path rel("/bar");
	path result = base + rel;
	return assert_true(result.getNormalizedPath() == "/foo/bar",
		"test_concat_right_leading_slash_no_double_slash", LINE_DATA());
}

int test_concat_left_root_no_double_slash()
{
	path base("/");
	path rel("/bar");
	path result = base + rel;
	return assert_true(result.getNormalizedPath() == "/bar",
		"test_concat_left_root_no_double_slash", LINE_DATA());
}

int test_concat_left_clean_path_trailing_slash_no_double_slash()
{
	path base("/foo/?x=1");
	path rel("/bar");
	path result = base + rel;
	return assert_true(result.getNormalizedPath() == "/foo/bar",
		"test_concat_left_clean_path_trailing_slash_no_double_slash", LINE_DATA());
}

int test_concat_invalid_left_operand_yields_invalid_result()
{
	path invalid("/../foo");
	path valid("/bar");
	path result = invalid + valid;
	return assert(403, result.validatePath(),
		"test_concat_invalid_left_operand_yields_invalid_result", LINE_DATA());
}

int test_concat_invalid_right_operand_yields_invalid_result()
{
	path valid("/foo");
	path invalid("/../bar");
	path result = valid + invalid;
	return assert(403, result.validatePath(),
		"test_concat_invalid_right_operand_yields_invalid_result", LINE_DATA());
}

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
	failures += test_normalize_relative_path_gets_leading_slash();

	failures += test_validate_path_valid_returns_zero();
	failures += test_validate_path_traversal_returns_403();
	failures += test_validate_path_empty_input_returns_403();

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

	failures += test_extension_simple_case();
	failures += test_extension_empty_when_no_dot();
	failures += test_extension_ignores_dot_in_directory_name();
	failures += test_extension_not_leaked_from_query_string();
	failures += test_extension_correct_alongside_query_string();

	failures += test_basename_simple_case();
	failures += test_basename_equals_filename_when_no_extension();
	failures += test_basename_root_path_is_empty();
	failures += test_basename_ignores_dot_in_directory_name();

	failures += test_concat_no_slashes_adds_separator();
	failures += test_concat_right_leading_slash_no_double_slash();
	failures += test_concat_left_root_no_double_slash();
	failures += test_concat_left_clean_path_trailing_slash_no_double_slash();
	failures += test_concat_invalid_left_operand_yields_invalid_result();
	failures += test_concat_invalid_right_operand_yields_invalid_result();

	messages::print();

	return failures;
}