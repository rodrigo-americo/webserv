/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_utils.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 10:08:48 by ighannam          #+#    #+#             */
/*   Updated: 2026/06/16 22:15:46 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

# include <iostream>
# include <string>

# include "utils.hpp"
# include "text.hpp"

# define LINE_DATA() ((std::stringstream() << __FILE__ << ":" << __LINE__).str())

std::string	formatLine(const std::string &test_line)
{
	return test_line.empty() ? "" : " " + text::color(200,200,200).underline().apply(test_line);
}

template <typename T>
int assert(const T& expected, const T& actual, const std::string& test_name, const std::string &test_line = "")
{
    if (expected != actual)
    {
		text::style	error_style = text::color(240, 100, 100);
		text::style	warn_style = text::color(240, 240, 100);
        std::cerr << text::style(error_style).bold().apply("FAILED: ")
			<< text::style(error_style).apply(test_name)
			<< formatLine(test_line)
            << text::style(warn_style).bold().apply("\nEXPECTED:  ")
			<< text::style(warn_style).underline().apply(utils::to_string(expected))
            << text::style(error_style).bold().apply("\nGOT:  ")
			<< text::style(error_style).strikethrough().apply(utils::to_string(actual))
			<< std::endl;
        return 1;
    }
	text::style	seccess_style = text::color(100, 240, 100);
    std::cout
		<< text::style(seccess_style).bold().apply("PASSED: ")
		<< text::style(seccess_style).apply(test_name)
		<<  formatLine(test_line)
		<< std::endl;
    return 0;
}

int	assert_true(bool value, const std::string& test_name, const std::string &test_line = "")
{
	return assert(true, value, test_name, test_line);
}

int	assert_false(bool value, const std::string& test_name)
{
	return assert(false, value, test_name);
}

#endif
