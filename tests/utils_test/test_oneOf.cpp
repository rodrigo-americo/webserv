/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_oneOf.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/18 13:19:05 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/18 13:20:12 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"
#include "test_utils.hpp"

// ── oneOf<T> ──────────────────────────────────────────────────────────────────

// Feature: valor presente no vetor retorna true
int test_oneOf_found()
{
	std::vector<int> opts;
	opts.push_back(1); opts.push_back(2); opts.push_back(3);
	return assert_true(utils::oneOf(opts, 2), "test_oneOf_found", LINE_DATA());
}

// Feature: valor ausente no vetor retorna false
int test_oneOf_not_found()
{
	std::vector<int> opts;
	opts.push_back(1); opts.push_back(2); opts.push_back(3);
	return assert_true(!utils::oneOf(opts, 5), "test_oneOf_not_found", LINE_DATA());
}

// Feature: vetor vazio sempre retorna false
int test_oneOf_empty_vector()
{
	std::vector<int> empty_opts;
	return assert_true(!utils::oneOf(empty_opts, 5), "test_oneOf_empty_vector", LINE_DATA());
}

// Feature: oneOf funciona com tipos não numéricos (std::string)
int test_oneOf_with_strings_found()
{
	std::vector<std::string> opts;
	opts.push_back("a"); opts.push_back("b");
	return assert_true(utils::oneOf(opts, std::string("b")), "test_oneOf_with_strings_found", LINE_DATA());
}

int test_oneOf_with_strings_not_found()
{
	std::vector<std::string> opts;
	opts.push_back("a"); opts.push_back("b");
	return assert_true(!utils::oneOf(opts, std::string("z")), "test_oneOf_with_strings_not_found", LINE_DATA());
}

// Feature: valor presente na primeira posição é encontrado
int test_oneOf_first_element()
{
	std::vector<int> opts;
	opts.push_back(7); opts.push_back(8);
	return assert_true(utils::oneOf(opts, 7), "test_oneOf_first_element", LINE_DATA());
}

// Feature: valor presente na última posição é encontrado
int test_oneOf_last_element()
{
	std::vector<int> opts;
	opts.push_back(7); opts.push_back(8); opts.push_back(9);
	return assert_true(utils::oneOf(opts, 9), "test_oneOf_last_element", LINE_DATA());
}

// ── main ──────────────────────────────────────────────────────────────────────

int main(int argc, char **argv)
{
	messages::settup(argc, argv);

	int failures = 0;

	// std::cout << "\n=== oneOf<T> ===\n";
	failures += test_oneOf_found();
	failures += test_oneOf_not_found();
	failures += test_oneOf_empty_vector();
	failures += test_oneOf_with_strings_found();
	failures += test_oneOf_with_strings_not_found();
	failures += test_oneOf_first_element();
	failures += test_oneOf_last_element();

	messages::print();

	return failures;
}
