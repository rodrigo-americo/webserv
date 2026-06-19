/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_number_double.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 18:09:58 by ighannam          #+#    #+#             */
/*   Updated: 2026/06/18 16:13:39 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "test_utils.hpp"
# include "schema.hpp"

int test_schema_double_basic_parse_pass()
{
    std::string str = "42.42";
    schema_double double_schema = schema::dbl().name("NAME");
    schema_result_double expected = schema_result_double::success(42.42);
    schema_result_double got = double_schema.parse(str);
    return assert(expected, got, "test_schema_double_basic_parse_pass", LINE_DATA());
}

int test_schema_double_basic_parse_fail()
{
    std::string str = "abc";
    schema_double double_schema = schema::dbl().name("NAME");
     schema_result_double expected =  schema_result_double::failure(schema_error("NAME", "ss.fail() Entry is abc, but it must be a number"));
     schema_result_double got = double_schema.parse(str);
    return assert(expected, got, "test_schema_double_basic_parse_fail", LINE_DATA());
}

int test_schema_double_not_valid_sufix_parse_fail()
{
    std::string str = "42.42abc";
    schema_double double_schema = schema::dbl().name("NAME");
    schema_result_double expected = schema_result_double::failure(schema_error("NAME", "!this->_sufix_optional && this->_sufixes.empty() && ss.peek() != std::stringstream::traits_type::eof() Entry is 42.42abc, but it must be a number"));
    schema_result_double got = double_schema.parse(str);
    return assert(expected, got, "test_schema_double_not_valid_sufix_parse_fail", LINE_DATA());
}

int test_schema_double_space_sufix_parse_fail()
{
    std::string str = "42.42 ";
    schema_double double_schema = schema::dbl().name("NAME");
    schema_result_double expected = schema_result_double::failure(schema_error("NAME", "!this->_sufix_optional && this->_sufixes.empty() && ss.peek() != std::stringstream::traits_type::eof() Entry is 42.42 , but it must be a number"));
    schema_result_double got = double_schema.parse(str);
    return assert(expected, got, "test_schema_double_space_sufix_parse_fail", LINE_DATA());
}

int test_schema_double_basic_min_fail()
{
    std::string str = "42.42";
    schema_double double_schema = schema::dbl().name("NAME").min(43.452);
    schema_result_double expected = schema_result_double::failure(schema_error("NAME", "Minimal value not reached. Is 42.42. Must be at least 43.452"));
    schema_result_double got = double_schema.parse(str);
    return assert(expected, got, "test_schema_double_not_valid_sufix_parse_fail", LINE_DATA());
}

int test_schema_double_basic_min_pass()
{
    std::string str = "42.42";
    schema_double double_schema = schema::dbl().name("NAME").min(42);
    schema_result_double expected = schema_result_double::success(42.42);
    schema_result_double got = double_schema.parse(str);
    return assert(expected, got, "test_schema_double_basic_parse_pass", LINE_DATA());
}

int test_schema_double_limit_min_pass()
{
    std::string str = "42.42";
    schema_double double_schema = schema::dbl().name("NAME").min(42.4200000000001);
    schema_result_double expected = schema_result_double::success(42.42);
    schema_result_double got = double_schema.parse(str);
    return assert(expected, got, "test_schema_double_basic_parse_pass", LINE_DATA());
}

int test_schema_double_basic_max_fail()
{
    std::string str = "42.42";
    schema_double double_schema = schema::dbl().name("NAME").max(41.452);
    schema_result_double expected = schema_result_double::failure(schema_error("NAME", "Maximal value overreached. Is 42.42. Must be at max 41.452"));
    schema_result_double got = double_schema.parse(str);
    return assert(expected, got, "test_schema_double_not_valid_sufix_parse_fail", LINE_DATA());
}

int test_schema_double_basic_max_pass()
{
    std::string str = "42.42";
    schema_double double_schema = schema::dbl().name("NAME").max(42.43);
    schema_result_double expected = schema_result_double::success(42.42);
    schema_result_double got = double_schema.parse(str);
    return assert(expected, got, "test_schema_double_basic_parse_pass", LINE_DATA());
}

int test_schema_double_limit_max_pass()
{
    std::string str = "42.42";
    schema_double double_schema = schema::dbl().name("NAME").max(42.4200000000001);
    schema_result_double expected = schema_result_double::success(42.42);
    schema_result_double got = double_schema.parse(str);
    return assert(expected, got, "test_schema_double_basic_parse_pass", LINE_DATA());
}

int test_schema_double_between_pass()
{
    std::string str = "42.42";
    schema_double double_schema = schema::dbl().name("NAME").between(40, 50);
    schema_result_double expected = schema_result_double::success(42.42);
    schema_result_double got = double_schema.parse(str);
    return assert(expected, got, "test_schema_double_basic_parse_pass", LINE_DATA());
}

int test_schema_double_between_fail()
{
    std::string str = "42.42";
    schema_double double_schema = schema::dbl().name("NAME").between(30, 40);
    schema_result_double expected = schema_result_double::failure(schema_error("NAME", "Maximal value overreached. Is 42.42. Must be at max 40"));
    schema_result_double got = double_schema.parse(str);
    return assert(expected, got, "test_schema_double_not_valid_sufix_parse_fail", LINE_DATA());
}

int test_schema_double_big_eps_min_pass()
{
    std::string str = "42.42";
    schema_double double_schema = schema::dbl().name("NAME").eps(3).min(44);
    schema_result_double expected = schema_result_double::success(42.42);
    schema_result_double got = double_schema.parse(str);
    return assert(expected, got, "test_schema_double_basic_parse_pass", LINE_DATA());
}


int main(int argc, char **argv)
{
	messages::settup(argc, argv);

	int failures = 0;

	// std::cout << "\n=== basic parse ===\n";
	failures += test_schema_double_basic_parse_pass();
	failures += test_schema_double_basic_parse_fail();
	failures += test_schema_double_not_valid_sufix_parse_fail();
	failures += test_schema_double_space_sufix_parse_fail();

	// std::cout << "\n=== min ===\n";
	failures += test_schema_double_basic_min_fail();
	failures += test_schema_double_basic_min_pass();
	failures += test_schema_double_limit_min_pass();

	// std::cout << "\n=== max ===\n";
	failures += test_schema_double_basic_max_pass();
	failures += test_schema_double_basic_max_fail();
	failures += test_schema_double_limit_max_pass();

	// std::cout << "\n=== between ===\n";
	failures += test_schema_double_between_pass();
	failures += test_schema_double_between_fail();

	// std::cout << "\n=== custom eps ===\n";
	failures += test_schema_double_big_eps_min_pass();

	messages::print();

	return failures;
}
