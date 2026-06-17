/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_bool.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/17 14:12:03 by ighannam          #+#    #+#             */
/*   Updated: 2026/06/17 15:35:29 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "test_utils.hpp"
# include "schema.hpp"

int test_schema_bool_true_pass_basic()
{
    std::string str = "True";
    schema_bool bool_schema = schema::boolean().truthy().name("NAME");
    schema_result_bool expected = schema_result_bool::success(true);
    schema_result_bool got = bool_schema.parse(str);
    return assert(expected, got, "test_schema_bool_true_pass_basic", LINE_DATA());
}

int test_schema_bool_true_all_caps_pass_basic()
{
    std::string str = "TRUE";
    schema_bool bool_schema = schema::boolean().truthy().name("NAME");
    schema_result_bool expected = schema_result_bool::success(true);
    schema_result_bool got = bool_schema.parse(str);
    return assert(expected, got, "test_schema_bool_true_all_caps_pass_basic", LINE_DATA());
}

int test_schema_bool_true_all_lower_pass_basic()
{
    std::string str = "true";
    schema_bool bool_schema = schema::boolean().truthy().name("NAME");
    schema_result_bool expected = schema_result_bool::success(true);
    schema_result_bool got = bool_schema.parse(str);
    return assert(expected, got, "test_schema_bool_true_all_lower_pass_basic", LINE_DATA());
}

int test_schema_bool_true_fail_invalid()
{
    std::string str = "tru";
    schema_bool bool_schema = schema::boolean().truthy().name("NAME");
    schema_result_bool expected = schema_result_bool::failure(schema_error("NAME", "tru must be valid."));
    schema_result_bool got = bool_schema.parse(str);
    return assert(expected, got, "test_schema_bool_true_fail_basic", LINE_DATA());
}

int test_schema_bool_true_fail_basic()
{
    std::string str = "false";
    schema_bool bool_schema = schema::boolean().truthy().name("NAME");
    schema_result_bool expected = schema_result_bool::failure(schema_error("NAME", "false must be true."));
    schema_result_bool got = bool_schema.parse(str);
    return assert(expected, got, "test_schema_bool_true_fail_basic", LINE_DATA());
}

int test_schema_bool_true_add_pass()
{
    std::string str = "YES";
    schema_bool bool_schema = schema::boolean().name("NAME").add_truthy("YES").truthy();
    schema_result_bool expected = schema_result_bool::success(true);
    schema_result_bool got = bool_schema.parse(str);
    return assert(expected, got, "test_schema_bool_true_add_pass", LINE_DATA());
}

int test_schema_bool_true_add_fail()
{
    std::string str = "YES";
    schema_bool bool_schema = schema::boolean().add_truthy("OK").truthy().name("NAME");
    schema_result_bool expected = schema_result_bool::failure(schema_error("NAME", "YES must be valid."));
    schema_result_bool got = bool_schema.parse(str);
    return assert(expected, got, "test_schema_bool_true_add_fail", LINE_DATA());
}

int test_schema_bool_false_pass_basic()
{
    std::string str = "False";
    schema_bool bool_schema = schema::boolean().falsy().name("NAME");
    schema_result_bool expected = schema_result_bool::success(false);
    schema_result_bool got = bool_schema.parse(str);
    return assert(expected, got, "test_schema_bool_false_pass_basic", LINE_DATA());
}

int test_schema_bool_false_all_caps_pass_basic()
{
    std::string str = "FALSE";
    schema_bool bool_schema = schema::boolean().falsy().name("NAME");
    schema_result_bool expected = schema_result_bool::success(false);
    schema_result_bool got = bool_schema.parse(str);
    return assert(expected, got, "test_schema_bool_false_all_caps_pass_basic", LINE_DATA());
}

int test_schema_bool_false_all_lower_pass_basic()
{
    std::string str = "false";
    schema_bool bool_schema = schema::boolean().falsy().name("NAME");
    schema_result_bool expected = schema_result_bool::success(false);
    schema_result_bool got = bool_schema.parse(str);
    return assert(expected, got, "test_schema_bool_false_all_lower_pass_basic", LINE_DATA());
}

int test_schema_bool_false_fail_basic()
{
    std::string str = "true";
    schema_bool bool_schema = schema::boolean().falsy().name("NAME");
    schema_result_bool expected = schema_result_bool::failure(schema_error("NAME", "true must be false."));
    schema_result_bool got = bool_schema.parse(str);
    return assert(expected, got, "test_schema_bool_false_fail_basic", LINE_DATA());
}

int test_schema_bool_false_fail_invalid()
{
    std::string str = "fal";
    schema_bool bool_schema = schema::boolean().falsy().name("NAME");
    schema_result_bool expected = schema_result_bool::failure(schema_error("NAME", "fal must be valid."));
    schema_result_bool got = bool_schema.parse(str);
    return assert(expected, got, "test_schema_bool_false_fail_basic", LINE_DATA());
}

int test_schema_bool_false_add_pass()
{
    std::string str = "NO";
    schema_bool bool_schema = schema::boolean().name("NAME").add_falsy("NO").falsy();
    schema_result_bool expected = schema_result_bool::success(false);
    schema_result_bool got = bool_schema.parse(str);
    return assert(expected, got, "test_schema_bool_false_add_pass", LINE_DATA());
}

int test_schema_bool_false_add_fail()
{
    std::string str = "NO";
    schema_bool bool_schema = schema::boolean().add_falsy("NOT").falsy().name("NAME");
    schema_result_bool expected = schema_result_bool::failure(schema_error("NAME", "NO must be valid."));
    schema_result_bool got = bool_schema.parse(str);
    return assert(expected, got, "test_schema_bool_false_add_fail", LINE_DATA());
}


int main()
{
    int failures = 0;

    std::cout << "\n=== truthy - basic ===\n";
    failures += test_schema_bool_true_pass_basic();
    failures += test_schema_bool_true_all_caps_pass_basic();
    failures += test_schema_bool_true_all_lower_pass_basic();
    failures += test_schema_bool_true_fail_basic();
    failures += test_schema_bool_true_fail_invalid();
    

    std::cout << "\n=== falsy - basic ===\n";
    failures += test_schema_bool_false_pass_basic();
    failures += test_schema_bool_false_all_caps_pass_basic();
    failures += test_schema_bool_false_all_lower_pass_basic();
    failures += test_schema_bool_false_fail_basic();
    failures += test_schema_bool_false_fail_invalid();

    std::cout << "\n=== add truthy ===\n";
    failures += test_schema_bool_true_add_pass();
    failures += test_schema_bool_true_add_fail();

    std::cout << "\n=== add falsy ===\n";
    failures += test_schema_bool_false_add_pass();
    failures += test_schema_bool_false_add_fail();
    
	return failures;
}