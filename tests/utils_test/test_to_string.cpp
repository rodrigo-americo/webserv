/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_to_string.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by sperado. O        #+#    #+#             */
/*   Updated: 2026/06/18 13:34:42 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "utils.hpp"
#include "test_utils.hpp"

#include <vector>
#include <string>

// ── to_string<T> (template genérico) ──────────────────────────────────────────

// Feature: to_string converte inteiro para string
int test_to_string_int()
{
    return assert(std::string("42"), utils::to_string(42), "test_to_string_int", LINE_DATA());
}

// Feature: to_string converte inteiro negativo para string
int test_to_string_negative_int()
{
    return assert(std::string("-42"), utils::to_string(-42), "test_to_string_negative_int", LINE_DATA());
}

// Feature: to_string converte double para string
int test_to_string_double()
{
    return assert(std::string("3.14"), utils::to_string(3.14), "test_to_string_double", LINE_DATA());
}

// Feature: to_string converte zero corretamente
int test_to_string_zero()
{
    return assert(std::string("0"), utils::to_string(0), "test_to_string_zero", LINE_DATA());
}

// Feature: to_string converte char (insere o caractere, não o código numérico,
// pois std::stringstream trata char como caractere via operator<<)
int test_to_string_char()
{
    return assert(std::string("A"), utils::to_string('A'), "test_to_string_char", LINE_DATA());
}

// ── to_string(bool) — overload não-template ───────────────────────────────────

// Feature: overload bool retorna "true" para true
int test_to_string_bool_true()
{
    return assert(std::string("true"), utils::to_string(true), "test_to_string_bool_true", LINE_DATA());
}

// Feature: overload bool retorna "false" para false
int test_to_string_bool_false()
{
    return assert(std::string("false"), utils::to_string(false), "test_to_string_bool_false", LINE_DATA());
}

// Feature: o template genérico também usa std::boolalpha, então chamado
// explicitamente como to_string<bool>(...) produz o mesmo resultado do
// overload — os dois caminhos são consistentes entre si (não há divergência
// de comportamento, apenas uma sobrecarga redundante).
int test_to_string_bool_template_consistent_with_overload()
{
    bool ok = utils::to_string<bool>(true)  == utils::to_string(true)
           && utils::to_string<bool>(false) == utils::to_string(false);
    return assert_true(ok, "test_to_string_bool_template_consistent_with_overload", LINE_DATA());
}

// ── main ──────────────────────────────────────────────────────────────────────

int main(int argc, char **argv)
{
	messages::settup(argc, argv);

	int failures = 0;

	// std::cout << "\n=== to_string<T> ===\n";
	failures += test_to_string_int();
	failures += test_to_string_negative_int();
	failures += test_to_string_double();
	failures += test_to_string_zero();
	failures += test_to_string_char();

	// std::cout << "\n=== to_string(bool) overload ===\n";
	failures += test_to_string_bool_true();
	failures += test_to_string_bool_false();
	failures += test_to_string_bool_template_consistent_with_overload();

	messages::print();

	return failures;
}
