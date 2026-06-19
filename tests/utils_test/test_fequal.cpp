/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_fequal.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/18 13:28:41 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/18 13:29:39 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"
#include "test_utils.hpp"

// ── fequal<T> (template genérico com eps default 1e-9) ────────────────────────

// Feature: valores idênticos são considerados iguais
int test_fequal_identical()
{
	return assert_true(utils::fequal(1.0, 1.0), "test_fequal_identical", LINE_DATA());
}

// Feature: valores dentro do epsilon padrão são considerados iguais
int test_fequal_within_default_eps()
{
	return assert_true(utils::fequal(1.0, 1.0000000001), "test_fequal_within_default_eps", LINE_DATA());
}

// Feature: valores fora do epsilon padrão são considerados diferentes
int test_fequal_outside_default_eps()
{
	return assert_true(!utils::fequal(1.0, 1.1), "test_fequal_outside_default_eps", LINE_DATA());
}

// Feature: eps customizado mais permissivo aceita diferença maior
int test_fequal_custom_eps_accepts()
{
	return assert_true(utils::fequal(1.0, 1.05, 0.1), "test_fequal_custom_eps_accepts", LINE_DATA());
}

// Feature: eps customizado rejeita diferença além do limite
int test_fequal_custom_eps_rejects()
{
	return assert_true(!utils::fequal(1.0, 1.2, 0.1), "test_fequal_custom_eps_rejects", LINE_DATA());
}

// Feature: a comparação é <= eps (inclusiva), então a diferença exatamente
// igual ao eps deve ser considerada igual
int test_fequal_exact_eps_boundary_inclusive()
{
	return assert_true(utils::fequal(1.0, 1.1, 0.1), "test_fequal_exact_eps_boundary_inclusive", LINE_DATA());
}

// Feature: valores negativos iguais são considerados iguais
int test_fequal_negative_identical()
{
	return assert_true(utils::fequal(-1.0, -1.0), "test_fequal_negative_identical", LINE_DATA());
}

// Feature: valores com sinais opostos e diferença grande são diferentes
int test_fequal_opposite_signs_differ()
{
	return assert_true(!utils::fequal(-1.0, 1.0), "test_fequal_opposite_signs_differ", LINE_DATA());
}

// Feature: fequal instanciado com int funciona, pois a subtração de inteiros
// é convertida para double na comparação com eps (double)
int test_fequal_with_int_type()
{
	bool ok = utils::fequal(5, 5) && !utils::fequal(5, 6);
	return assert_true(ok, "test_fequal_with_int_type", LINE_DATA());
}

// ── fequal(float, float, double) — overload com eps default 1e-4 ──────────────

// Feature: overload float usa eps maior (1e-4) por padrão, então aceita
// diferenças que o overload double (eps 1e-9) rejeitaria
int test_fequal_float_overload_default_eps()
{
	float a = 1.0f, b = 1.00001f; // diferença ~1e-5, dentro de 1e-4
	return assert_true(utils::fequal(a, b), "test_fequal_float_overload_default_eps", LINE_DATA());
}

// Feature: overload float rejeita diferença maior que o eps default (1e-4)
int test_fequal_float_overload_rejects_large_diff()
{
	float a = 1.0f, b = 1.001f; // diferença ~1e-3, fora de 1e-4
	return assert_true(!utils::fequal(a, b), "test_fequal_float_overload_rejects_large_diff", LINE_DATA());
}

// Feature: overload float aceita eps customizado também
int test_fequal_float_overload_custom_eps()
{
	float a = 1.0f, b = 1.001f;
	return assert_true(utils::fequal(a, b, 0.01), "test_fequal_float_overload_custom_eps", LINE_DATA());
}

// ── main ──────────────────────────────────────────────────────────────────────

int main(int argc, char **argv)
{
	messages::settup(argc, argv);

	int failures = 0;


	// std::cout << "\n=== fequal<T> ===\n";
	failures += test_fequal_identical();
	failures += test_fequal_within_default_eps();
	failures += test_fequal_outside_default_eps();
	failures += test_fequal_custom_eps_accepts();
	failures += test_fequal_custom_eps_rejects();
	failures += test_fequal_exact_eps_boundary_inclusive();
	failures += test_fequal_negative_identical();
	failures += test_fequal_opposite_signs_differ();
	failures += test_fequal_with_int_type();

	// std::cout << "\n=== fequal(float,float,double) overload ===\n";
	failures += test_fequal_float_overload_default_eps();
	failures += test_fequal_float_overload_rejects_large_diff();
	failures += test_fequal_float_overload_custom_eps();

	messages::print();

	return failures;
}
