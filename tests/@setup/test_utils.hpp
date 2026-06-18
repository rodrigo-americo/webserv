/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_utils.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 10:08:48 by ighannam          #+#    #+#             */
/*   Updated: 2026/06/18 13:35:43 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

# include <iostream>
# include <sstream>
# include <vector>
# include <string>

# include "utils.hpp"
# include "text.hpp"

class messages
{
private:
	static std::vector<std::string>	_errors;
	static std::vector<std::string>	_successes;
	static bool						_verbose;

	static void	_printErrors()
	{
		for (size_t i = 0; i < _errors.size(); i++)
			std::cerr << _errors[i];
	}

	static void	_printSuccesses()
	{
		for (size_t i = 0; i < _successes.size(); i++)
			std::cout << _successes[i];
	}

public:

	static void	settup(int argc, char **argv)
	{
		_verbose = false;
		if (argc >= 2)
			if (std::string(argv[1]) == "--verbose")
				_verbose = true;
	}

	static void	addError(const std::string &error) { _errors.push_back(error); }
	static void	addSuccess(const std::string &success) { _successes.push_back(success); }
	static void	print()
	{
		if (_verbose)
			_printSuccesses();
		_printErrors();
		if (!_verbose && !_errors.size())
		{
			std::cout << "\033[32m" << _successes.size() << " tests passed.\033[0m ";
			std::cout << "\033[31m" << _errors.size() << " test(s) failed.\033[0m\n";
			return;
		}
		std::cout << "──────────────────────────────────────────\n";
		std::cout << "\033[32m" << _successes.size() << " tests passed.\033[0m\n";
		std::cout << "\033[31m" << _errors.size() << " test(s) failed.\033[0m\n\n";
	}
};

# define LINE_DATA() ((std::stringstream() << __FILE__ << ":" << __LINE__).str())

inline std::string	formatLine(const std::string &test_line)
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
		std::stringstream	ss;
		ss << text::style(error_style).bold().apply("FAILED: ")
			<< text::style(error_style).apply(test_name)
			<< formatLine(test_line)
			<< text::style(warn_style).bold().apply("\nEXPECTED:  ")
			<< text::style(warn_style).underline().apply(utils::to_string(expected))
			<< text::style(error_style).bold().apply("\nGOT:  ")
			<< text::style(error_style).strikethrough().apply(utils::to_string(actual))
			<< std::endl;
		messages::addError(ss.str());
		return 1;
	}
	text::style	seccess_style = text::color(100, 240, 100);
	std::stringstream	ss;
		ss
		<< text::style(seccess_style).bold().apply("PASSED: ")
		<< text::style(seccess_style).apply(test_name)
		<<  formatLine(test_line)
		<< std::endl;
	messages::addSuccess(ss.str());
	return 0;
}

inline int	assert_true(bool value, const std::string& test_name, const std::string &test_line = "")
{
	return assert(true, value, test_name, test_line);
}

inline int	assert_false(bool value, const std::string& test_name)
{
	return assert(false, value, test_name);
}

#endif
