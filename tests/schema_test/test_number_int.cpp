/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_number_int.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 15:37:21 by ighannam          #+#    #+#             */
/*   Updated: 2026/06/17 20:52:36 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "test_utils.hpp"
# include "schema.hpp"

#include <vector>

int test_schema_int_basic_parse_pass()
{
    std::string str = "42";
    schema_int int_schema = schema::integer().name("NAME");
    schema_result_int expected = schema_result_int::success(42);
    schema_result_int got = int_schema.parse(str);
    return assert(expected, got, "test_schema_int_basic_parse_pass", LINE_DATA());
}

int test_schema_int_basic_parse_fail()
{
    std::string str = "abc";
    schema_int int_schema = schema::integer().name("NAME");
    schema_result_int expected = schema_result_int::failure(schema_error("NAME", "ss.fail() Entry is abc, but it must be a number"));
    schema_result_int got = int_schema.parse(str);
    return assert(expected, got, "test_schema_int_basic_parse_fail", LINE_DATA());
}

int test_schema_int_not_valid_sufix_parse_fail()
{
    std::string str = "42abc";
    schema_int int_schema = schema::integer().name("NAME");
    schema_result_int expected = schema_result_int::failure(schema_error("NAME", "!this->_sufix_optional && this->_sufixes.empty() && ss.peek() != std::stringstream::traits_type::eof() Entry is 42abc, but it must be a number"));
    schema_result_int got = int_schema.parse(str);
    return assert(expected, got, "test_schema_int_not_valid_sufix_parse_fail", LINE_DATA());
}

int test_schema_int_space_sufix_parse_fail()
{
    std::string str = "42 ";
    schema_int int_schema = schema::integer().name("NAME");
    schema_result_int expected = schema_result_int::failure(schema_error("NAME", "!this->_sufix_optional && this->_sufixes.empty() && ss.peek() != std::stringstream::traits_type::eof() Entry is 42 , but it must be a number"));
    schema_result_int got = int_schema.parse(str);
    return assert(expected, got, "test_schema_int_space_sufix_parse_fail", LINE_DATA());
}

int test_schema_int_basic_min_pass()
{
    std::string str = "42";
    schema_int int_schema = schema::integer().name("NAME").min(40);
    schema_result_int expected = schema_result_int::success(42);
    schema_result_int got = int_schema.parse(str);
    return assert(expected, got, "test_schema_int_basic_min_pass", LINE_DATA());
}

int test_schema_int_basic_min_fail()
{
    std::string str = "42";
    schema_int int_schema = schema::integer().name("NAME").min(50);
    schema_result_int expected = schema_result_int::failure(schema_error("NAME", "Minimal value not reached. Is 42. Must be at least 50"));
    schema_result_int got = int_schema.parse(str);
    return assert(expected, got, "test_schema_int_basic_min_fail", LINE_DATA());
}

int test_schema_int_limit_min_pass()
{
    std::string str = "42";
    schema_int int_schema = schema::integer().name("NAME").min(42);
    schema_result_int expected = schema_result_int::success(42);
    schema_result_int got = int_schema.parse(str);
    return assert(expected, got, "test_schema_int_basic_min_pass", LINE_DATA());
}

int test_schema_int_basic_max_pass()
{
    std::string str = "42";
    schema_int int_schema = schema::integer().name("NAME").max(50);
    schema_result_int expected = schema_result_int::success(42);
    schema_result_int got = int_schema.parse(str);
    return assert(expected, got, "test_schema_int_basic_max_pass", LINE_DATA());
}

int test_schema_int_basic_max_fail()
{
    std::string str = "42";
    schema_int int_schema = schema::integer().name("NAME").max(40);
    schema_result_int expected = schema_result_int::failure(schema_error("NAME", "Maximal value overreached. Is 42. Must be at max 40"));
    schema_result_int got = int_schema.parse(str);
    return assert(expected, got, "test_schema_int_basic_max_fail", LINE_DATA());
}

int test_schema_int_limit_max_pass()
{
    std::string str = "42";
    schema_int int_schema = schema::integer().name("NAME").max(42);
    schema_result_int expected = schema_result_int::success(42);
    schema_result_int got = int_schema.parse(str);
    return assert(expected, got, "test_schema_int_limit_max_pass", LINE_DATA());
}

int test_schema_int_between_pass()
{
    std::string str = "42";
    schema_int int_schema = schema::integer().name("NAME").between(30, 50);
    schema_result_int expected = schema_result_int::success(42);
    schema_result_int got = int_schema.parse(str);
    return assert(expected, got, "test_schema_int_between_pass", LINE_DATA());
}

int test_schema_int_between_fail()
{
    std::string str = "42";
    schema_int int_schema = schema::integer().name("NAME").between(42, 30);
    schema_result_int expected = schema_result_int::failure(schema_error("NAME", "Maximal value overreached. Is 42. Must be at max 30"));
    schema_result_int got = int_schema.parse(str);
    return assert(expected, got, "test_schema_int_basic_max_fail", LINE_DATA());
}

int test_schema_int_oneOf_pass()
{
    std::string str = "42";
    int arr[] = {1, 2, 42, 35};
    std::vector<int> options(arr, arr + 4);
    schema_int int_schema = schema::integer().name("NAME").oneOf(options);
    schema_result_int expected = schema_result_int::success(42);
    schema_result_int got = int_schema.parse(str);
    return assert(expected, got, "test_schema_int_oneOf_pass", LINE_DATA());
}

int test_schema_int_oneOf_fail()
{
    std::string str = "42";
    int arr[] = {1, 2, 3};
    std::vector<int> options(arr, arr + 3);
    schema_int int_schema = schema::integer().name("NAME").oneOf(options);
    schema_result_int expected = schema_result_int::failure(schema_error("NAME", "Value 42 is not one of the given options."));
    schema_result_int got = int_schema.parse(str);
    return assert(expected, got, "test_schema_int_oneOf_fail", LINE_DATA());
}

int test_schema_int_sufix_pass()
{
    std::string str = "42KB";
    schema_int int_schema = schema::integer().name("NAME").sufix("KB", 1000);
    schema_result_int expected = schema_result_int::success(42000);
    schema_result_int got = int_schema.parse(str);
    return assert(expected, got, "test_schema_int_sufix_pass", LINE_DATA());
}
int test_schema_int_sufix_fail()
{
    std::string str = "42";
    schema_int int_schema = schema::integer().name("NAME").sufix("KB", 1000);
    schema_result_int expected = schema_result_int::failure(schema_error("NAME", "Sufix 42 is invalid, must be one of 'KB'."));
    schema_result_int got = int_schema.parse(str);
    return assert(expected, got, "test_schema_int_sufix_fail", LINE_DATA());
}

int test_schema_int_sufix_optional_with_sufix_pass()
{
    std::string str = "42";
    schema_int int_schema = schema::integer().name("NAME").sufixOptional(1000);
    schema_result_int expected = schema_result_int::success(42000);
    schema_result_int got = int_schema.parse(str);
    return assert(expected, got, "test_schema_int_sufix_optional_with_sufix_pass", LINE_DATA());
}

int test_schema_int_sufix_optional_without_sufix_pass()
{
    std::string str = "42KB";
    schema_int int_schema = schema::integer().name("NAME").sufixOptional(1000).sufix("KB", 1000);
    schema_result_int expected = schema_result_int::success(42000);
    schema_result_int got = int_schema.parse(str);
    return assert(expected, got, "test_schema_int_sufix_optional_without_sufix_pass", LINE_DATA());
}

int test_schema_int_sufix_case_insensitive_pass()
{
    std::string str = "42KB";
    schema_int int_schema = schema::integer().name("NAME").caseInsensitive().sufix("KB", 1000);
    schema_result_int expected = schema_result_int::success(42000);
    schema_result_int got = int_schema.parse(str);
    return assert(expected, got, "test_schema_int_sufix_optional_with_sufix_case_insensitive_pass", LINE_DATA());
}


int main()
{
    int failures = 0;

    std::cout << "\n=== basic parse ===\n";
    failures += test_schema_int_basic_parse_pass();
    failures += test_schema_int_basic_parse_fail();
    failures += test_schema_int_not_valid_sufix_parse_fail();
    failures += test_schema_int_space_sufix_parse_fail();

    std::cout << "\n=== min ===\n";
    failures += test_schema_int_basic_min_fail();
    failures += test_schema_int_basic_min_pass();
    failures += test_schema_int_limit_min_pass();

    std::cout << "\n=== max ===\n";
    failures += test_schema_int_basic_max_pass();
    failures += test_schema_int_basic_max_fail();
    failures += test_schema_int_limit_max_pass();

    std::cout << "\n=== between ===\n";
    failures += test_schema_int_between_pass();
    failures += test_schema_int_between_fail();

    std::cout << "\n=== oneOf ===\n";
    failures += test_schema_int_oneOf_pass();
    failures += test_schema_int_oneOf_fail();
    

    std::cout << "\n=== sufix ===\n";
    failures += test_schema_int_sufix_pass();
    failures += test_schema_int_sufix_fail();

    std::cout << "\n=== sufixOptional ===\n";
    failures += test_schema_int_sufix_optional_with_sufix_pass();
    failures += test_schema_int_sufix_optional_without_sufix_pass();

    std::cout << "\n=== caseInsensitive ===\n";
    failures += test_schema_int_sufix_case_insensitive_pass();
	
    return failures;
}