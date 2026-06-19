/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_lerp.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/18 13:17:04 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/18 15:08:55 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"
#include "test_utils.hpp"

// ── lerp(double, double, double) ──────────────────────────────────────────────

// Feature: factor 0.5 retorna o ponto médio
int test_lerp_double_midpoint()
{
	return assert_true(utils::lerp(0.0, 10.0, 0.5) == 5.0, "test_lerp_double_midpoint", LINE_DATA());
}

// Feature: factor 0 retorna exatamente n1
int test_lerp_double_factor_zero()
{
	return assert_true(utils::lerp(0.0, 10.0, 0.0) == 0.0, "test_lerp_double_factor_zero", LINE_DATA());
}

// Feature: factor 1 retorna exatamente n2
int test_lerp_double_factor_one()
{
	return assert_true(utils::lerp(0.0, 10.0, 1.0) == 10.0, "test_lerp_double_factor_one", LINE_DATA());
}

// Feature: lerp funciona com intervalos que não começam em zero
int test_lerp_double_nonzero_start()
{
	return assert_true(utils::lerp(5.0, 15.0, 0.5) == 10.0, "test_lerp_double_nonzero_start", LINE_DATA());
}

// Feature: factor > 1 extrapola além de n2 (a função não limita o factor)
int test_lerp_double_extrapolates_beyond_one()
{
	return assert_true(utils::lerp(0.0, 10.0, 1.5) == 15.0, "test_lerp_double_extrapolates_beyond_one", LINE_DATA());
}

// Feature: factor negativo extrapola antes de n1
int test_lerp_double_extrapolates_negative()
{
	return assert_true(utils::lerp(0.0, 10.0, -0.5) == -5.0, "test_lerp_double_extrapolates_negative", LINE_DATA());
}

// ── lerp(vector<double>, vector<double>, double) ──────────────────────────────

// Feature: tamanhos diferentes retornam vetor vazio (guarda de segurança)
int test_lerp_vector_mismatched_sizes_returns_empty()
{
	std::vector<double> v1; v1.push_back(1); v1.push_back(2);
	std::vector<double> v2; v2.push_back(1);
	std::vector<double> result = utils::lerp(v1, v2, 0.5);
	return assert_true(result.empty(), "test_lerp_vector_mismatched_sizes_returns_empty", LINE_DATA());
}

// Feature: vetores vazios (mesmo tamanho, ambos 0) retornam vetor vazio sem
// crash
int test_lerp_vector_both_empty()
{
	std::vector<double> v1, v2;
	std::vector<double> result = utils::lerp(v1, v2, 0.5);
	return assert_true(result.empty(), "test_lerp_vector_both_empty", LINE_DATA());
}

// Feature: vetores de tamanho 1 interpolam corretamente (não há índice
// diferente de 0 para revelar o bug nesse caso específico)
int test_lerp_vector_size_one()
{
	std::vector<double> v1; v1.push_back(10);
	std::vector<double> v2; v2.push_back(20);
	std::vector<double> result = utils::lerp(v1, v2, 0.5);
	return assert_true(result.size() == 1 && result[0] == 15.0,
					"test_lerp_vector_size_one", LINE_DATA());
}

// Feature: o vetor resultado tem o mesmo tamanho dos vetores de entrada
int test_lerp_vector_result_size_matches_input()
{
	std::vector<double> v1; v1.push_back(0); v1.push_back(100); v1.push_back(200);
	std::vector<double> v2; v2.push_back(10); v2.push_back(110); v2.push_back(210);
	std::vector<double> result = utils::lerp(v1, v2, 0.5);
	return assert_true(result.size() == 3, "test_lerp_vector_result_size_matches_input", LINE_DATA());
}


int test_lerp_vector_returns_midpoint_between_two_vectors()
{
	std::vector<double> v1; v1.push_back(0);   v1.push_back(100); v1.push_back(200);
	std::vector<double> v2; v2.push_back(10);  v2.push_back(110); v2.push_back(210);
	std::vector<double> result = utils::lerp(v1, v2, 0.5);

	std::stringstream	ss_result;
	ss_result << result[0] << ", " << result[1] << ", " << result[2];

	std::stringstream	ss_expected;
	ss_expected << 5 << ", " << 105 << ", " << 205;

	return assert(ss_expected.str(), ss_result.str(), "test_lerp_vector_returns_midpoint_between_two_vectors", LINE_DATA());
}

// ── main ──────────────────────────────────────────────────────────────────────

int main(int argc, char **argv)
{
	messages::settup(argc, argv);

	int failures = 0;

// std::cout << "\n=== lerp(double,double,double) ===\n";
	failures += test_lerp_double_midpoint();
	failures += test_lerp_double_factor_zero();
	failures += test_lerp_double_factor_one();
	failures += test_lerp_double_nonzero_start();
	failures += test_lerp_double_extrapolates_beyond_one();
	failures += test_lerp_double_extrapolates_negative();

	// std::cout << "\n=== lerp(vector<double>,vector<double>,double) ===\n";
	failures += test_lerp_vector_mismatched_sizes_returns_empty();
	failures += test_lerp_vector_both_empty();
	failures += test_lerp_vector_size_one();
	failures += test_lerp_vector_result_size_matches_input();
	failures += test_lerp_vector_returns_midpoint_between_two_vectors();

	messages::print();

	return failures;
}
