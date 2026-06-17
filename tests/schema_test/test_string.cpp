/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_string.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 10:18:08 by ighannam          #+#    #+#             */
/*   Updated: 2026/06/17 14:11:41 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "test_utils.hpp"
# include "schema.hpp"

#include <vector>

int test_schema_str_minLength_fail_basic()
{
    std::string str = "Isadora";
    schema_string name_schema =  schema::string().minLength(20).name("NAME");
    schema_result_string expected = schema_result_string::failure(schema_error("NAME", "Minimal lenght not reached. Must be >= 20"));
    schema_result_string got = name_schema.parse(str);
    return assert(expected, got, "test_schema_str_minLength_fail_basic", LINE_DATA());
}

int test_schema_str_minLength_fail_limit()
{
    std::string str = "Isadora";
    schema_string name_schema =  schema::string().minLength(8).name("NAME");
    schema_result_string expected = schema_result_string::failure(schema_error("NAME", "Minimal lenght not reached. Must be >= 8"));
    schema_result_string got = name_schema.parse(str);
    return assert(expected, got, "test_schema_str_minLength_fail_limit", LINE_DATA());
}

int test_schema_str_minLength_pass_basic()
{
    std::string str = "Isadora";
    schema_string name_schema =  schema::string().minLength(2).name("NAME");
    schema_result_string expected = schema_result_string::success("Isadora");
    schema_result_string got = name_schema.parse(str);
    return assert(expected, got, "test_schema_str_minLength_pass_basic", LINE_DATA());
}

int test_schema_str_minLength_pass_limit()
{
    std::string str = "Isadora";
    schema_string name_schema =  schema::string().minLength(7).name("NAME");
    schema_result_string expected = schema_result_string::success("Isadora");
    schema_result_string got = name_schema.parse(str);
    return assert(expected, got, "test_schema_str_minLength_pass_limit", LINE_DATA());
}

int test_schema_str_maxLength_fail_basic()
{
    std::string str = "Isadora";
    schema_string name_schema =  schema::string().maxLength(2).name("NAME");
    schema_result_string expected = schema_result_string::failure(schema_error("NAME", "Max length overreached. Must be <= 2"));
    schema_result_string got = name_schema.parse(str);
    return assert(expected, got, "test_schema_str_maxLength_fail_basic", LINE_DATA());
}

int test_schema_str_maxLength_fail_limit()
{
    std::string str = "Isadora";
    schema_string name_schema =  schema::string().maxLength(6).name("NAME");
    schema_result_string expected = schema_result_string::failure(schema_error("NAME", "Max length overreached. Must be <= 6"));
    schema_result_string got = name_schema.parse(str);
    return assert(expected, got, "test_schema_str_maxLength_fail_limit", LINE_DATA());
}

int test_schema_str_maxLength_pass_basic()
{
    std::string str = "Isadora";
    schema_string name_schema =  schema::string().maxLength(20).name("NAME");
    schema_result_string expected = schema_result_string::success("Isadora");
    schema_result_string got = name_schema.parse(str);
    return assert(expected, got, "test_schema_str_maxLength_pass_basic", LINE_DATA());
}

int test_schema_str_maxLength_pass_limit()
{
    std::string str = "Isadora";
    schema_string name_schema =  schema::string().maxLength(7).name("NAME");
    schema_result_string expected = schema_result_string::success("Isadora");
    schema_result_string got = name_schema.parse(str);
    return assert(expected, got, "test_schema_str_maxLength_pass_limit", LINE_DATA());
}

int test_schema_str_startsWith_pass_basic()
{
    std::string str = "Isadora";
    schema_string name_schema =  schema::string().startsWith("Isa").name("NAME");
    schema_result_string expected = schema_result_string::success("Isadora");
    schema_result_string got = name_schema.parse(str);
    return assert(expected, got, "test_schema_str_startsWith_pass_basic", LINE_DATA());
}

int test_schema_str_startsWith_fail_basic()
{
    std::string str = "Isadora";
    schema_string name_schema =  schema::string().startsWith("dora").name("NAME");
    schema_result_string expected = schema_result_string::failure(schema_error("NAME", "Prefix incorrect. Must be: dora"));
    schema_result_string got = name_schema.parse(str);
    return assert(expected, got, "test_schema_str_startsWith_fail_basic", LINE_DATA());
}

int test_schema_str_startsWith_fail_prefix_greather_than_str()
{
    std::string str = "Isadora";
    schema_string name_schema =  schema::string().startsWith("IsadoraTeste").name("NAME");
    schema_result_string expected = schema_result_string::failure(schema_error("NAME", "Prefix incorrect. Must be: IsadoraTeste"));
    schema_result_string got = name_schema.parse(str);
    return assert(expected, got, "test_schema_str_startsWith_fail_prefix_greather_than_str", LINE_DATA());
}

int test_schema_str_noChars_pass_basic()
{
    std::string str = "Isadora";
    schema_string name_schema =  schema::string().noChars("te").name("NAME");
    schema_result_string expected = schema_result_string::success("Isadora");
    schema_result_string got = name_schema.parse(str);
    return assert(expected, got, "test_schema_str_noChars_pass_basic", LINE_DATA());
}

int test_schema_str_noChars_fail_basic()
{
    std::string str = "Isadora";
    schema_string name_schema =  schema::string().noChars("ted").name("NAME");
    schema_result_string expected = schema_result_string::failure(schema_error("NAME", "Forbidden char found. String must not have: ted"));
    schema_result_string got = name_schema.parse(str);
    return assert(expected, got, "test_schema_str_noChars_fail_basic", LINE_DATA());
}

int test_schema_str_noChars_pass_empty_str()
{
    std::string str = "Isadora";
    schema_string name_schema =  schema::string().noChars("").name("NAME");
    schema_result_string expected = schema_result_string::success("Isadora");
    schema_result_string got = name_schema.parse(str);
    return assert(expected, got, "test_schema_str_noChars_pass_empty_str", LINE_DATA());
}

int test_schema_str_minLength_maxLength_fail()
{
    std::string str = "Isadora";
    schema_string name_schema =  schema::string().minLength(20).maxLength(5).name("NAME");
    std::vector<schema_error> errs;
    errs.push_back(schema_error("NAME", "Minimal lenght not reached. Must be >= 20"));
    errs.push_back(schema_error("NAME", "Max length overreached. Must be <= 5"));
    schema_result_string expected = schema_result_string::failure(errs);
    schema_result_string got = name_schema.parse(str);
    return assert(expected, got, "test_schema_str_noChars_fail_basic", LINE_DATA());
}

int test_schema_str_minLength_maxLength_fail_custom_msg()
{
    std::string str = "Isadora";
    schema_string name_schema =  schema::string().minLength(20, "test message: min not reached").maxLength(5, "test message: max not reached").name("NAME");
    std::vector<schema_error> errs;
    errs.push_back(schema_error("NAME", "test message: min not reached"));
    errs.push_back(schema_error("NAME", "test message: max not reached"));
    schema_result_string expected = schema_result_string::failure(errs);
    schema_result_string got = name_schema.parse(str);
    return assert(expected, got, "test_schema_str_minLength_maxLength_fail_custom_msg", LINE_DATA());
}

int main()
{
    int failures = 0;

    std::cout << "\n=== minLength ===\n";
    failures += test_schema_str_minLength_fail_basic();
    failures += test_schema_str_minLength_pass_basic();
    failures += test_schema_str_minLength_fail_limit();
    failures += test_schema_str_minLength_pass_limit();

    std::cout << "\n=== maxLength ===\n";
    failures += test_schema_str_maxLength_fail_basic();
    failures += test_schema_str_maxLength_pass_basic();
    failures += test_schema_str_maxLength_fail_limit();
    failures += test_schema_str_maxLength_pass_limit();

    std::cout << "\n=== startsWith ===\n";
    failures += test_schema_str_startsWith_pass_basic();
    failures += test_schema_str_startsWith_fail_basic();
    failures += test_schema_str_startsWith_fail_prefix_greather_than_str();

    std::cout << "\n=== noChars ===\n";
    failures += test_schema_str_noChars_pass_basic();
    failures += test_schema_str_noChars_fail_basic();
    failures += test_schema_str_noChars_pass_empty_str();

    std::cout << "\n=== minLength + maxLength ===\n";
    failures += test_schema_str_minLength_maxLength_fail();

    std::cout << "\n=== custom error msg ===\n";
    failures += test_schema_str_minLength_maxLength_fail_custom_msg();
    
	return failures;
}
