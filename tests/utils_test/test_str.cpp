/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_str.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 00:48:44 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/18 14:20:02 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "test_utils.hpp"
#include "str.hpp"

#include <sstream>
#include <vector>

typedef utils::str str;

// ── Construtores ──────────────────────────────────────────────────────────────

int test_ctor_default()
{
	str s;
	return assert_true(s.empty() && s.size() == 0, "test_ctor_default", LINE_DATA());
}

int test_ctor_from_cstr()
{
	str s("hello");
	return assert_true(s == "hello", "test_ctor_from_cstr", LINE_DATA());
}

int test_ctor_from_stdstring()
{
	std::string std_s = "world";
	str s(std_s);
	return assert_true(s == "world", "test_ctor_from_stdstring", LINE_DATA());
}

int test_ctor_fill()
{
	str s(5, 'x');
	return assert_true(s == "xxxxx" && s.size() == 5, "test_ctor_fill", LINE_DATA());
}

int test_ctor_copy()
{
	str a("abc");
	str b(a);
	return assert_true(b == "abc", "test_ctor_copy", LINE_DATA());
}

int test_ctor_copy_independent()
{
	str a("abc");
	str b(a);
	b += "X";
	return assert_true(a == "abc", "test_ctor_copy_independent", LINE_DATA());
}

// ── Accessors básicos ─────────────────────────────────────────────────────────

int test_size()
{
	str s("hello");
	return assert_true(s.size() == 5, "test_size", LINE_DATA());
}

int test_empty_true()
{
	str s;
	return assert_true(s.empty(), "test_empty_true", LINE_DATA());
}

int test_empty_false()
{
	str s("x");
	return assert_true(!s.empty(), "test_empty_false", LINE_DATA());
}

int test_clear()
{
	str s("abc");
	s.clear();
	return assert_true(s.empty(), "test_clear", LINE_DATA());
}

int test_c_str()
{
	str s("hi");
	return assert_true(std::string(s.c_str()) == "hi", "test_c_str", LINE_DATA());
}

int test_string_accessor()
{
	str s("test");
	return assert_true(s.string() == "test", "test_string_accessor", LINE_DATA());
}

int test_subscript_read()
{
	str s("abc");
	return assert_true(s[0] == 'a' && s[2] == 'c', "test_subscript_read", LINE_DATA());
}

int test_subscript_write()
{
	str s("abc");
	s[1] = 'X';
	return assert_true(s == "aXc", "test_subscript_write", LINE_DATA());
}

int test_data()
{
	str s("abc");
	char *d = s.data();
	d[0] = 'Z';
	return assert_true(s == "Zbc", "test_data", LINE_DATA());
}

// ── Iteradores ────────────────────────────────────────────────────────────────

int test_begin_end()
{
	str s("abc");
	std::string built(s.begin(), s.end());
	return assert_true(built == "abc", "test_begin_end", LINE_DATA());
}

int test_const_begin_end()
{
	const str s("xyz");
	std::string built(s.begin(), s.end());
	return assert_true(built == "xyz", "test_const_begin_end", LINE_DATA());
}

// ── Operadores de atribuição ──────────────────────────────────────────────────

int test_assign_str()
{
	str a("hello");
	str b;
	b = a;
	return assert_true(b == "hello", "test_assign_str", LINE_DATA());
}

int test_assign_self()
{
	str a("hello");
	a.operator=(a);
	return assert_true(a == "hello", "test_assign_self", LINE_DATA());
}

int test_assign_stdstring()
{
	str s;
	s = std::string("world");
	return assert_true(s == "world", "test_assign_stdstring", LINE_DATA());
}

int test_assign_cstr()
{
	str s;
	s = "hi";
	return assert_true(s == "hi", "test_assign_cstr", LINE_DATA());
}

int test_assign_char()
{
	str s;
	s = 'A';
	return assert_true(s == "A", "test_assign_char", LINE_DATA());
}

// ── Operadores de concatenação ────────────────────────────────────────────────

int test_pluseq_str()
{
	str s("foo");
	s += str("bar");
	return assert_true(s == "foobar", "test_pluseq_str", LINE_DATA());
}

int test_pluseq_stdstring()
{
	str s("foo");
	s += std::string("bar");
	return assert_true(s == "foobar", "test_pluseq_stdstring", LINE_DATA());
}

int test_pluseq_cstr()
{
	str s("foo");
	s += "bar";
	return assert_true(s == "foobar", "test_pluseq_cstr", LINE_DATA());
}

int test_pluseq_char()
{
	str s("foo");
	s += '!';
	return assert_true(s == "foo!", "test_pluseq_char", LINE_DATA());
}

int test_plus_str()
{
	str a("foo");
	str b = a + str("bar");
	return assert_true(b == "foobar" && a == "foo", "test_plus_str", LINE_DATA());
}

int test_plus_stdstring()
{
	str a("foo");
	str b = a + std::string("bar");
	return assert_true(b == "foobar", "test_plus_stdstring", LINE_DATA());
}

int test_plus_cstr()
{
	str a("foo");
	str b = a + "bar";
	return assert_true(b == "foobar", "test_plus_cstr", LINE_DATA());
}

int test_plus_char()
{
	str a("foo");
	str b = a + '!';
	return assert_true(b == "foo!", "test_plus_char", LINE_DATA());
}

int test_stdstring_plus_str()
{
	std::string a = "hello ";
	str b = "world";
	std::string result = a + b;
	return assert_true(result == "hello world", "test_stdstring_plus_str", LINE_DATA());
}

// ── Operadores de comparação ──────────────────────────────────────────────────

int test_eq_str()    { return assert_true(str("a") == str("a"), "test_eq_str", LINE_DATA()); }
int test_neq_str()   { return assert_true(str("a") != str("b"), "test_neq_str", LINE_DATA()); }
int test_lt_str()    { return assert_true(str("a") <  str("b"), "test_lt_str", LINE_DATA()); }
int test_lte_str()   { return assert_true(str("a") <= str("a"), "test_lte_str", LINE_DATA()); }
int test_gt_str()    { return assert_true(str("b") >  str("a"), "test_gt_str", LINE_DATA()); }
int test_gte_str()   { return assert_true(str("b") >= str("b"), "test_gte_str", LINE_DATA()); }

int test_eq_stdstring()  { return assert_true(str("a") == std::string("a"), "test_eq_stdstring", LINE_DATA()); }
int test_neq_stdstring() { return assert_true(str("a") != std::string("b"), "test_neq_stdstring", LINE_DATA()); }
int test_lt_stdstring()  { return assert_true(str("a") <  std::string("b"), "test_lt_stdstring", LINE_DATA()); }
int test_gt_stdstring()  { return assert_true(str("b") >  std::string("a"), "test_gt_stdstring", LINE_DATA()); }

int test_eq_cstr()  { return assert_true(str("a") == "a", "test_eq_cstr", LINE_DATA()); }
int test_neq_cstr() { return assert_true(str("a") != "b", "test_neq_cstr", LINE_DATA()); }
int test_lt_cstr()  { return assert_true(str("a") <  "b", "test_lt_cstr", LINE_DATA()); }
int test_gt_cstr()  { return assert_true(str("b") >  "a", "test_gt_cstr", LINE_DATA()); }

// ── compare() ────────────────────────────────────────────────────────────────

int test_compare_equal()
{
	return assert_true(str("abc").compare(str("abc")) == 0, "test_compare_equal", LINE_DATA());
}

int test_compare_less()
{
	return assert_true(str("abc").compare(str("abd")) < 0, "test_compare_less", LINE_DATA());
}

int test_compare_greater()
{
	return assert_true(str("abd").compare(str("abc")) > 0, "test_compare_greater", LINE_DATA());
}

int test_compare_cstr()
{
	return assert_true(str("abc").compare("abc") == 0, "test_compare_cstr", LINE_DATA());
}

// ── substr() ─────────────────────────────────────────────────────────────────

int test_substr_full()
{
	str s("hello");
	return assert_true(s.substr(0) == "hello", "test_substr_full", LINE_DATA());
}

int test_substr_partial()
{
	str s("hello");
	return assert_true(s.substr(1, 3) == "ell", "test_substr_partial", LINE_DATA());
}

int test_substr_to_end()
{
	str s("hello");
	return assert_true(s.substr(3) == "lo", "test_substr_to_end", LINE_DATA());
}

// ── trim() ────────────────────────────────────────────────────────────────────

int test_trim_leading_spaces()
{
	// CORRETO: leading spaces são removidos corretamente.
	str s("   hello");
	s.trim();
	return assert_true(s == "hello", "test_trim_leading_spaces", LINE_DATA());
}

int test_trim_trailing_spaces()
{
	str s("hello   ");
	s.trim();
	return assert(str("hello"), s, "test_trim_trailing_spaces", LINE_DATA());
}

int test_trim_both_sides()
{
	str s("  hello  ");
	s.trim();
	return assert_true(s == "hello", "test_trim_both_sides", LINE_DATA());
}

int test_trim_tabs_and_newlines()
{
	str s("\t\nhello\n\t");
	s.trim();
	return assert_true(s == "hello", "test_trim_tabs_and_newlines", LINE_DATA());
}

int test_trim_only_spaces_becomes_empty()
{
	// CORRETO: string só de espaços → esvazia (first==last dispara o clear()).
	str s("   ");
	s.trim();
	return assert_true(s.empty(), "test_trim_only_spaces_becomes_empty", LINE_DATA());
}

int test_trim_no_spaces()
{
	// CORRETO: sem espaços → sem alteração.
	str s("hello");
	s.trim();
	return assert_true(s == "hello", "test_trim_no_spaces", LINE_DATA());
}

int test_trim_const()
{
	str s("  hello  ");
	str t = s.trim_const();
	return assert_true(s == "  hello  " && t == "hello",
					"test_trim_const", LINE_DATA());
}

// ── tolower() / toupper() ─────────────────────────────────────────────────────

int test_tolower_basic()
{
	str s("Hello World");
	s.tolower();
	return assert_true(s == "hello world", "test_tolower_basic", LINE_DATA());
}

int test_tolower_already_lower()
{
	str s("hello");
	s.tolower();
	return assert_true(s == "hello", "test_tolower_already_lower", LINE_DATA());
}

int test_tolower_const_does_not_modify()
{
	str s("HELLO");
	str t = s.tolower_const();
	return assert_true(s == "HELLO" && t == "hello", "test_tolower_const_does_not_modify", LINE_DATA());
}

int test_toupper_basic()
{
	str s("Hello World");
	s.toupper();
	return assert_true(s == "HELLO WORLD", "test_toupper_basic", LINE_DATA());
}

int test_toupper_already_upper()
{
	str s("HELLO");
	s.toupper();
	return assert_true(s == "HELLO", "test_toupper_already_upper", LINE_DATA());
}

int test_toupper_const_does_not_modify()
{
	str s("hello");
	str t = s.toupper_const();
	return assert_true(s == "hello" && t == "HELLO", "test_toupper_const_does_not_modify", LINE_DATA());
}

// ── split() ───────────────────────────────────────────────────────────────────

int test_split_char_basic()
{
	str s("a,b,c");
	std::vector<str> parts = s.split(',');
	return assert_true(parts.size() == 3 &&
					parts[0] == "a" && parts[1] == "b" && parts[2] == "c",
					"test_split_char_basic", LINE_DATA());
}

int test_split_char_single_item()
{
	str s("hello");
	std::vector<str> parts = s.split(',');
	return assert_true(parts.size() == 1 && parts[0] == "hello",
					"test_split_char_single_item", LINE_DATA());
}

int test_split_string_basic()
{
	str s("one::two::three");
	std::vector<str> parts = s.split("::");
	return assert_true(parts.size() == 3 &&
					parts[0] == "one" && parts[1] == "two" && parts[2] == "three",
					"test_split_string_basic", LINE_DATA());
}

int test_split_spaces()
{
	str s("hello world foo");
	std::vector<str> parts = s.split(' ');
	return assert_true(parts.size() == 3 &&
					parts[0] == "hello" && parts[1] == "world" && parts[2] == "foo",
					"test_split_spaces", LINE_DATA());
}

// ── join() ────────────────────────────────────────────────────────────────────

int test_join_with_sep()
{
	std::vector<str> v;
	v.push_back("a"); v.push_back("b"); v.push_back("c");
	str result = str::join(v, str(","));
	return assert_true(result == "a,b,c", "test_join_with_sep", LINE_DATA());
}

int test_join_no_sep()
{
	std::vector<str> v;
	v.push_back("foo"); v.push_back("bar");
	str result = str::join(v, str(""));
	return assert_true(result == "foobar", "test_join_no_sep", LINE_DATA());
}

int test_join_single_element()
{
	std::vector<str> v;
	v.push_back("only");
	str result = str::join(v, str("-"));
	return assert_true(result == "only", "test_join_single_element", LINE_DATA());
}

int test_join_with_prefix()
{
	std::vector<str> v;
	v.push_back("b"); v.push_back("c");
	str prefix("a");
	str result = prefix.join_with_const(v, str("-"));
	return assert_true(result == "ab-c", "test_join_with_prefix", LINE_DATA());
}

int test_join_with_modifies_self()
{
	std::vector<str> v;
	v.push_back("b"); v.push_back("c");
	str s("a");
	s.join_with(v, str("-"));
	return assert_true(s == "ab-c", "test_join_with_modifies_self", LINE_DATA());
}

// ── contains() ────────────────────────────────────────────────────────────────

int test_contains_str_found()
{
	str s("hello world");
	return assert_true(s.contains(str("o")), "test_contains_str_found", LINE_DATA());
}

int test_contains_str_not_found()
{
	str s("hello");
	return assert_true(!s.contains(str("z")), "test_contains_str_not_found", LINE_DATA());
}

int test_contains_char_found()
{
	str s("hello");
	return assert_true(s.contains('e'), "test_contains_char_found", LINE_DATA());
}

int test_contains_char_not_found()
{
	str s("hello");
	return assert_true(!s.contains('z'), "test_contains_char_not_found", LINE_DATA());
}

// ── starts_with() / ends_with() ───────────────────────────────────────────────

int test_starts_with_true()
{
	str s("hello world");
	return assert_true(s.starts_with(str("h")), "test_starts_with_true", LINE_DATA());
}

int test_starts_with_false()
{
	str s("hello world");
	return assert_true(!s.starts_with(str("world")), "test_starts_with_false", LINE_DATA());
}

int test_ends_with_true()
{
	str s("hello world");
	return assert_true(s.ends_with(str("d")), "test_ends_with_true", LINE_DATA());
}

int test_ends_with_false()
{
	str s("hello world");
	return assert_true(!s.ends_with(str("hello")), "test_ends_with_false", LINE_DATA());
}

// ── find() ────────────────────────────────────────────────────────────────────

int test_find_str_found()
{
	str s("hello world");
	return assert_true(s.find(str("world")) == 6, "test_find_str_found", LINE_DATA());
}

int test_find_str_not_found()
{
	str s("hello");
	return assert_true(s.find(str("xyz")) == str::npos, "test_find_str_not_found", LINE_DATA());
}

int test_find_char_found()
{
	str s("hello");
	return assert_true(s.find('l') == 2, "test_find_char_found", LINE_DATA());
}

int test_find_with_offset()
{
	str s("abcabc");
	return assert_true(s.find('a', 1) == 3, "test_find_with_offset", LINE_DATA());
}

// --- rfind() ------------------------------------------------------------------

// Feature: rfind encontra a última ocorrência de utils::str
int test_rfind_str()
{
	utils::str text("abc abc abc");

	return assert((size_t)8, text.rfind(utils::str("abc")),
		"test_rfind_str", LINE_DATA());
}

// Feature: rfind retorna npos quando não encontra utils::str
int test_rfind_str_not_found()
{
	str text("abc abc abc");

	return assert(str::npos, text.rfind(str("xyz")),
		"test_rfind_str_not_found", LINE_DATA());
}

// Feature: rfind respeita o parâmetro pos
int test_rfind_str_with_pos()
{
	str text("abc abc abc");

	return assert((size_t)4, text.rfind(str("abc"), 7),
		"test_rfind_str_with_pos", LINE_DATA());
}

// Feature: rfind encontra a última ocorrência de char
int test_rfind_char()
{
	str text("banana");

	return assert((size_t)5, text.rfind('a'),
		"test_rfind_char", LINE_DATA());
}

// Feature: rfind encontra a última ocorrência de std::string
int test_rfind_std_string()
{
	str text("hello world hello");

	return assert((size_t)12, text.rfind(std::string("hello")),
		"test_rfind_std_string", LINE_DATA());
}

// Feature: rfind retorna npos para char inexistente
int test_rfind_char_not_found()
{
	str text("banana");

	return assert(str::npos, text.rfind('z'),
		"test_rfind_char_not_found", LINE_DATA());
}

// Feature: rfind em string vazia retorna npos
int test_rfind_empty_string()
{
	str text("");

	return assert(str::npos, text.rfind(str("abc")),
		"test_rfind_empty_string", LINE_DATA());
}

// Feature: rfind de string vazia retorna o último índice válido
int test_rfind_search_empty_string()
{
	str text("abc");

	return assert((size_t)3, text.rfind(str("")),
		"test_rfind_search_empty_string", LINE_DATA());
}

// ── find_first_of() ───────────────────────────────────────────────────────────

int test_find_first_of_char()
{
	str s("hello");
	return assert_true(s.find_first_of('l') == 2, "test_find_first_of_char", LINE_DATA());
}

int test_find_first_of_str_charset()
{
	// find_first_of(str) usa std::string::find_first_of = busca qualquer
	// char do conjunto. "lo" como conjunto → primeiro 'l' está em índice 2.
	str s("hello");
	return assert_true(s.find_first_of(str("lo")) == 2, "test_find_first_of_str_charset", LINE_DATA());
}

int test_find_first_not_of_char()
{
	str s("aaabbb");
	return assert_true(s.find_first_not_of('a') == 3, "test_find_first_not_of_char", LINE_DATA());
}

int test_find_last_of_char()
{
	str s("hello");
	return assert_true(s.find_last_of('l') == 3, "test_find_last_of_char", LINE_DATA());
}

int test_find_last_not_of_char()
{
	str s("hello");
	return assert_true(s.find_last_not_of('l') == 4, "test_find_last_not_of_char", LINE_DATA());
}

int test_find_last_of_str_correct()
{
	// O overload find_last_of(str, pos=npos) usa pos=npos → funciona corretamente.
	str s("hello");
	return assert_true(s.find_last_of(str("l")) == 3, "test_find_last_of_str_correct", LINE_DATA());
}

// ── replace_first() ───────────────────────────────────────────────────────────

int test_replace_first_same_size()
{
	str s("hello");
	s.replace_first(str("l"), str("X"));
	// size() ainda é 5 (std::string mantém o tamanho), mas o conteúdo interno
	// tem um '\0' embutido em índice 3.
	return assert(str("heXlo"), s, "test_replace_first_same_size", LINE_DATA());
}

int test_replace_first_not_found()
{
	str s("hello");
	s.replace_first(str("z"), str("X"));
	return assert_true(s == "hello", "test_replace_first_not_found", LINE_DATA());
}

int test_replace_first_const_does_not_modify()
{
	str s("hello");
	str t = s.replace_first_const(str("z"), str("X"));
	return assert_true(s == "hello" && t == "hello",
					"test_replace_first_const_does_not_modify", LINE_DATA());
}

// ── insert() ─────────────────────────────────────────────────────────────────

int test_insert_at_beginning()
{
	str s("world");
	s.insert(0, str("hello "));
	return assert_true(s == "hello world", "test_insert_at_beginning", LINE_DATA());
}

int test_insert_at_end()
{
	str s("hello");
	s.insert(s.size(), str(" world"));
	return assert_true(s == "hello world", "test_insert_at_end", LINE_DATA());
}

int test_insert_in_middle()
{
	str s("helo");
	s.insert(3, str("l"));
	return assert_true(s == "hello", "test_insert_in_middle", LINE_DATA());
}

// ── remove() ─────────────────────────────────────────────────────────────────

int test_remove_char()
{
	str s("hello");
	s.remove('l');
	return assert_true(s == "helo", "test_remove_char", LINE_DATA());
}

int test_remove_char_not_found()
{
	// CORRETO: char não encontrado → sem alteração.
	str s("hello");
	s.remove('z');
	return assert_true(s == "hello", "test_remove_char_not_found", LINE_DATA());
}

int test_remove_str()
{
	str s("xaxb");
	s.remove(str("ab"));
	return assert(str("xaxb"), s, "test_remove_str", LINE_DATA());
}

// ── erase() ───────────────────────────────────────────────────────────────────

int test_erase_single()
{
	str s("hello");
	s.erase(s.begin());
	return assert_true(s == "ello", "test_erase_single", LINE_DATA());
}

int test_erase_range()
{
	str s("hello");
	s.erase(s.begin(), s.begin() + 2);
	return assert_true(s == "llo", "test_erase_range", LINE_DATA());
}

// ── to<T>() / str_to() ────────────────────────────────────────────────────────

int test_to_int_valid()
{
	str s("42");
	int val = 0;
	bool ok = s.to(val);
	// val é convertido corretamente para 42, mas ok é false
	return assert_true(ok && val == 42, "test_to_int_valid", LINE_DATA());
}

int test_to_int_invalid()
{
	// CORRETO: string não conversível retorna false.
	str s("abc");
	int val = 0;
	bool ok = s.to(val);
	return assert_true(!ok, "test_to_int_invalid", LINE_DATA());
}

int test_to_double_valid()
{
	str s("3.14");
	double val = 0;
	bool ok = s.to(val);
	return assert_true(ok && val > 3.13 && val < 3.15, "test_to_double_valid", LINE_DATA());
}

int test_to_with_rest()
{
	// CORRETO: a sobrecarga to(T, rest) não usa str_to e funciona normalmente.
	str s("42 rest");
	int val = 0;
	std::string rest;
	s.to(val, rest);
	return assert_true(val == 42 && rest == "rest", "test_to_with_rest", LINE_DATA());
}

int test_str_to_partial()
{
	str s("42abc");
	int val = 0;
	bool ok = s.to(val);
	return assert_true(!ok, "test_str_to_partial", LINE_DATA());
}

// ── ostream / istream operators ───────────────────────────────────────────────

int test_ostream_operator()
{
	str s("hello");
	std::ostringstream oss;
	oss << s;
	return assert_true(oss.str() == "hello", "test_ostream_operator", LINE_DATA());
}

int test_istream_operator()
{
	str s;
	std::istringstream iss("world");
	iss >> s;
	return assert_true(s == "world", "test_istream_operator", LINE_DATA());
}

// ── reserve() ────────────────────────────────────────────────────────────────

int test_reserve_does_not_change_content()
{
	str s("hello");
	s.reserve(1000);
	return assert_true(s == "hello" && s.size() == 5,
					"test_reserve_does_not_change_content", LINE_DATA());
}

// ── main ──────────────────────────────────────────────────────────────────────

int main(int argc, char **argv)
{
	messages::settup(argc, argv);

	int failures = 0;

	// std::cout << "\n=== Construtores ===\n";
	failures += test_ctor_default();
	failures += test_ctor_from_cstr();
	failures += test_ctor_from_stdstring();
	failures += test_ctor_fill();
	failures += test_ctor_copy();
	failures += test_ctor_copy_independent();

	// std::cout << "\n=== Accessors básicos ===\n";
	failures += test_size();
	failures += test_empty_true();
	failures += test_empty_false();
	failures += test_clear();
	failures += test_c_str();
	failures += test_string_accessor();
	failures += test_subscript_read();
	failures += test_subscript_write();
	failures += test_data();

	// std::cout << "\n=== Iteradores ===\n";
	failures += test_begin_end();
	failures += test_const_begin_end();

	// std::cout << "\n=== Operadores de atribuição ===\n";
	failures += test_assign_str();
	failures += test_assign_self();
	failures += test_assign_stdstring();
	failures += test_assign_cstr();
	failures += test_assign_char();

	// std::cout << "\n=== Concatenação ===\n";
	failures += test_pluseq_str();
	failures += test_pluseq_stdstring();
	failures += test_pluseq_cstr();
	failures += test_pluseq_char();
	failures += test_plus_str();
	failures += test_plus_stdstring();
	failures += test_plus_cstr();
	failures += test_plus_char();
	failures += test_stdstring_plus_str();

	// std::cout << "\n=== Comparação ===\n";
	failures += test_eq_str();
	failures += test_neq_str();
	failures += test_lt_str();
	failures += test_lte_str();
	failures += test_gt_str();
	failures += test_gte_str();
	failures += test_eq_stdstring();
	failures += test_neq_stdstring();
	failures += test_lt_stdstring();
	failures += test_gt_stdstring();
	failures += test_eq_cstr();
	failures += test_neq_cstr();
	failures += test_lt_cstr();
	failures += test_gt_cstr();

	// std::cout << "\n=== compare() ===\n";
	failures += test_compare_equal();
	failures += test_compare_less();
	failures += test_compare_greater();
	failures += test_compare_cstr();

	// std::cout << "\n=== substr() ===\n";
	failures += test_substr_full();
	failures += test_substr_partial();
	failures += test_substr_to_end();

	// std::cout << "\n=== trim() ===\n";
	failures += test_trim_leading_spaces();
	failures += test_trim_trailing_spaces();
	failures += test_trim_both_sides();
	failures += test_trim_tabs_and_newlines();
	failures += test_trim_only_spaces_becomes_empty();
	failures += test_trim_no_spaces();
	failures += test_trim_const();

	// std::cout << "\n=== tolower() / toupper() ===\n";
	failures += test_tolower_basic();
	failures += test_tolower_already_lower();
	failures += test_tolower_const_does_not_modify();
	failures += test_toupper_basic();
	failures += test_toupper_already_upper();
	failures += test_toupper_const_does_not_modify();

	// std::cout << "\n=== split() ===\n";
	failures += test_split_char_basic();
	failures += test_split_char_single_item();
	failures += test_split_string_basic();
	failures += test_split_spaces();

	// std::cout << "\n=== join() ===\n";
	failures += test_join_with_sep();
	failures += test_join_no_sep();
	failures += test_join_single_element();
	failures += test_join_with_prefix();
	failures += test_join_with_modifies_self();

	// std::cout << "\n=== contains() ===\n";
	failures += test_contains_str_found();
	failures += test_contains_str_not_found();
	failures += test_contains_char_found();
	failures += test_contains_char_not_found();

	// std::cout << "\n=== starts_with() / ends_with() ===\n";
	failures += test_starts_with_true();
	failures += test_starts_with_false();
	failures += test_ends_with_true();
	failures += test_ends_with_false();

	// std::cout << "\n=== find() ===\n";
	failures += test_find_str_found();
	failures += test_find_str_not_found();
	failures += test_find_char_found();
	failures += test_find_with_offset();

	// std::cout << "\n=== rfind() ===\n";
	test_rfind_str();
	test_rfind_str_not_found();
	test_rfind_str_with_pos();
	test_rfind_char();
	test_rfind_std_string();
	test_rfind_char_not_found();
	test_rfind_empty_string();
	test_rfind_search_empty_string();

	// std::cout << "\n=== find_first/last_of() ===\n";
	failures += test_find_first_of_char();
	failures += test_find_first_of_str_charset();
	failures += test_find_first_not_of_char();
	failures += test_find_last_of_char();
	failures += test_find_last_not_of_char();
	failures += test_find_last_of_str_correct();

	// std::cout << "\n=== replace_first() ===\n";
	failures += test_replace_first_same_size();
	failures += test_replace_first_not_found();
	failures += test_replace_first_const_does_not_modify();

	// std::cout << "\n=== insert() ===\n";
	failures += test_insert_at_beginning();
	failures += test_insert_at_end();
	failures += test_insert_in_middle();

	// std::cout << "\n=== remove() ===\n";
	failures += test_remove_char();
	failures += test_remove_char_not_found();
	failures += test_remove_str();

	// std::cout << "\n=== erase() ===\n";
	failures += test_erase_single();
	failures += test_erase_range();

	// std::cout << "\n=== to<T>() / str_to() ===\n";
	failures += test_to_int_valid();
	failures += test_to_int_invalid();
	failures += test_to_double_valid();
	failures += test_to_with_rest();
	failures += test_str_to_partial();

	// std::cout << "\n=== ostream / istream ===\n";
	failures += test_ostream_operator();
	failures += test_istream_operator();

	// std::cout << "\n=== reserve() ===\n";
	failures += test_reserve_does_not_change_content();

	messages::print();

	return failures;
}
