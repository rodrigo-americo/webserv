/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_arg_amount_validator.cpp                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/21 11:06:15 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/21 11:06:17 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "test_utils.hpp"
#include "ArgAmountValidator.hpp"

// ── helpers ──────────────────────────────────────────────────────────────────

static ParserToken make_name(const std::string &content, ParserTokenType::type type)
{
	return ParserToken("test.conf", 1, 0, content, type);
}

static std::vector<ParserToken> make_values(size_t n)
{
	std::vector<ParserToken> values;
	for (size_t i = 0; i < n; i++)
		values.push_back(ParserToken("test.conf", 1, 0, "v", ParserTokenType::PT_WORD));
	return values;
}

// ── ArgAmountValidator::validate — tipo não registrado ────────────────────────

int test_validate_unregistered_type_generates_error()
{
	ArgAmountValidator v;
	Directive d(make_name("custom", ParserTokenType::PT_WORD));
	v.validate(d);
	std::vector<std::string> errs;
	v.dumpErrorsOn(errs);
	bool ok = errs.size() == 1
		&& errs[0].find("not found on arg_amount_validations") != std::string::npos;
	return assert_true(ok, "test_validate_unregistered_type_generates_error", LINE_DATA());
}

int test_validate_unregistered_type_mentions_type_name()
{
	ArgAmountValidator v;
	Directive d(make_name("x", ParserTokenType::PT_WORD));
	v.validate(d);
	std::vector<std::string> errs;
	v.dumpErrorsOn(errs);
	bool ok = !errs.empty() && errs[0].find("PT_WORD") != std::string::npos;
	return assert_true(ok, "test_validate_unregistered_type_mentions_type_name", LINE_DATA());
}

// ── ArgAmountValidator::validate — dentro do range ────────────────────────────

int test_validate_exact_count_within_range_passes()
{
	ArgAmountValidator v;
	v.addValidation(ParserTokenType::PT_LISTEN, schema::unsigned_integer().between(1, 1).name("listen"));
	Directive d(make_name("listen", ParserTokenType::PT_LISTEN), make_values(1));
	v.validate(d);
	std::vector<std::string> errs;
	v.dumpErrorsOn(errs);
	return assert_true(errs.empty(), "test_validate_exact_count_within_range_passes", LINE_DATA());
}

int test_validate_min_boundary_passes()
{
	ArgAmountValidator v;
	v.addValidation(ParserTokenType::PT_LOCATION, schema::unsigned_integer().between(1, 2).name("location"));
	Directive d(make_name("location", ParserTokenType::PT_LOCATION), make_values(1));
	v.validate(d);
	std::vector<std::string> errs;
	v.dumpErrorsOn(errs);
	return assert_true(errs.empty(), "test_validate_min_boundary_passes", LINE_DATA());
}

int test_validate_max_boundary_passes()
{
	ArgAmountValidator v;
	v.addValidation(ParserTokenType::PT_LOCATION, schema::unsigned_integer().between(1, 2).name("location"));
	Directive d(make_name("location", ParserTokenType::PT_LOCATION), make_values(2));
	v.validate(d);
	std::vector<std::string> errs;
	v.dumpErrorsOn(errs);
	return assert_true(errs.empty(), "test_validate_max_boundary_passes", LINE_DATA());
}

int test_validate_zero_args_with_between_zero_zero_passes()
{
	ArgAmountValidator v;
	v.addValidation(ParserTokenType::PT_HTTP, schema::unsigned_integer().between(0, 0).name("http"));
	Directive d(make_name("http", ParserTokenType::PT_HTTP), make_values(0));
	v.validate(d);
	std::vector<std::string> errs;
	v.dumpErrorsOn(errs);
	return assert_true(errs.empty(), "test_validate_zero_args_with_between_zero_zero_passes", LINE_DATA());
}

// ── ArgAmountValidator::validate — fora do range ──────────────────────────────

int test_validate_too_few_args_generates_error()
{
	ArgAmountValidator v;
	v.addValidation(ParserTokenType::PT_LISTEN, schema::unsigned_integer().between(1, 1).name("listen"));
	Directive d(make_name("listen", ParserTokenType::PT_LISTEN), make_values(0));
	v.validate(d);
	std::vector<std::string> errs;
	v.dumpErrorsOn(errs);
	return assert_true(errs.size() == 1, "test_validate_too_few_args_generates_error", LINE_DATA());
}

int test_validate_too_many_args_generates_error()
{
	ArgAmountValidator v;
	v.addValidation(ParserTokenType::PT_ROOT, schema::unsigned_integer().between(1, 1).name("root"));
	Directive d(make_name("root", ParserTokenType::PT_ROOT), make_values(2));
	v.validate(d);
	std::vector<std::string> errs;
	v.dumpErrorsOn(errs);
	return assert_true(errs.size() == 1, "test_validate_too_many_args_generates_error", LINE_DATA());
}

int test_validate_zero_args_with_between_one_one_fails()
{
	ArgAmountValidator v;
	v.addValidation(ParserTokenType::PT_PID, schema::unsigned_integer().between(1, 1).name("pid"));
	Directive d(make_name("pid", ParserTokenType::PT_PID), make_values(0));
	v.validate(d);
	std::vector<std::string> errs;
	v.dumpErrorsOn(errs);
	return assert_true(errs.size() == 1, "test_validate_zero_args_with_between_one_one_fails", LINE_DATA());
}

int test_validate_one_arg_with_between_zero_zero_fails()
{
	ArgAmountValidator v;
	v.addValidation(ParserTokenType::PT_HTTP, schema::unsigned_integer().between(0, 0).name("http"));
	Directive d(make_name("http", ParserTokenType::PT_HTTP), make_values(1));
	v.validate(d);
	std::vector<std::string> errs;
	v.dumpErrorsOn(errs);
	return assert_true(errs.size() == 1, "test_validate_one_arg_with_between_zero_zero_fails", LINE_DATA());
}

// ── ArgAmountValidator::validate — funciona para Block, não só Directive ─────

int test_validate_accepts_block_node()
{
	ArgAmountValidator v;
	v.addValidation(ParserTokenType::PT_LOCATION, schema::unsigned_integer().between(1, 2).name("location"));
	Block b(make_name("location", ParserTokenType::PT_LOCATION), make_values(1));
	v.validate(b);
	std::vector<std::string> errs;
	v.dumpErrorsOn(errs);
	return assert_true(errs.empty(), "test_validate_accepts_block_node", LINE_DATA());
}

int test_validate_block_out_of_range_generates_error()
{
	ArgAmountValidator v;
	v.addValidation(ParserTokenType::PT_UPSTREAM, schema::unsigned_integer().between(1, 1).name("upstream"));
	Block b(make_name("upstream", ParserTokenType::PT_UPSTREAM), make_values(0));
	v.validate(b);
	std::vector<std::string> errs;
	v.dumpErrorsOn(errs);
	return assert_true(errs.size() == 1, "test_validate_block_out_of_range_generates_error", LINE_DATA());
}

// ── dumpErrorsOn — acumula no destino e limpa o interno ───────────────────────

int test_dumpErrorsOn_appends_to_destination()
{
	ArgAmountValidator v;
	v.addValidation(ParserTokenType::PT_LISTEN, schema::unsigned_integer().between(1, 1).name("listen"));
	Directive d(make_name("listen", ParserTokenType::PT_LISTEN), make_values(0));
	v.validate(d);

	std::vector<std::string> destine;
	destine.push_back("pre-existing error");
	v.dumpErrorsOn(destine);
	bool ok = destine.size() == 2 && destine[0] == "pre-existing error";
	return assert_true(ok, "test_dumpErrorsOn_appends_to_destination", LINE_DATA());
}

int test_dumpErrorsOn_clears_internal_list()
{
	ArgAmountValidator v;
	v.addValidation(ParserTokenType::PT_LISTEN, schema::unsigned_integer().between(1, 1).name("listen"));
	Directive d(make_name("listen", ParserTokenType::PT_LISTEN), make_values(0));
	v.validate(d); // 1 erro interno

	std::vector<std::string> first_dump;
	v.dumpErrorsOn(first_dump);

	std::vector<std::string> second_dump;
	v.dumpErrorsOn(second_dump); // sem novo validate() -> deve estar vazio

	bool ok = first_dump.size() == 1 && second_dump.empty();
	return assert_true(ok, "test_dumpErrorsOn_clears_internal_list", LINE_DATA());
}

int test_dumpErrorsOn_with_no_errors_does_not_modify_destination()
{
	ArgAmountValidator v;
	v.addValidation(ParserTokenType::PT_LISTEN, schema::unsigned_integer().between(1, 1).name("listen"));
	Directive d(make_name("listen", ParserTokenType::PT_LISTEN), make_values(1)); // válido
	v.validate(d);

	std::vector<std::string> destine;
	destine.push_back("existing");
	v.dumpErrorsOn(destine);
	bool ok = destine.size() == 1 && destine[0] == "existing";
	return assert_true(ok, "test_dumpErrorsOn_with_no_errors_does_not_modify_destination", LINE_DATA());
}

// ── addValidation — sobrescrita de regra para o mesmo tipo ────────────────────

int test_addValidation_overwrites_previous_rule_for_same_type()
{
	ArgAmountValidator v;
	v.addValidation(ParserTokenType::PT_LISTEN, schema::unsigned_integer().between(1, 1).name("listen v1"));
	v.addValidation(ParserTokenType::PT_LISTEN, schema::unsigned_integer().between(2, 2).name("listen v2"));

	// Com a regra v2 (exatamente 2), 1 argumento deve falhar agora.
	Directive d(make_name("listen", ParserTokenType::PT_LISTEN), make_values(1));
	v.validate(d);
	std::vector<std::string> errs;
	v.dumpErrorsOn(errs);
	return assert_true(errs.size() == 1, "test_addValidation_overwrites_previous_rule_for_same_type", LINE_DATA());
}

// ── ArgAmountValidatorBuilder::add — encadeamento fluente ─────────────────────

int test_builder_add_returns_self_for_chaining()
{
	ArgAmountValidatorBuilder builder;
	ArgAmountValidatorBuilder &ref = builder.add(ParserTokenType::PT_LISTEN,
		schema::unsigned_integer().between(1, 1).name("listen"));
	return assert_true(&ref == &builder, "test_builder_add_returns_self_for_chaining", LINE_DATA());
}

int test_builder_chained_adds_all_take_effect()
{
	ArgAmountValidator v = ArgAmountValidatorBuilder()
		.add(ParserTokenType::PT_LISTEN, schema::unsigned_integer().between(1, 1).name("listen"))
		.add(ParserTokenType::PT_ROOT, schema::unsigned_integer().between(1, 1).name("root"))
		.build();

	Directive listen(make_name("listen", ParserTokenType::PT_LISTEN), make_values(1));
	Directive root(make_name("root", ParserTokenType::PT_ROOT), make_values(1));
	v.validate(listen);
	v.validate(root);
	std::vector<std::string> errs;
	v.dumpErrorsOn(errs);
	return assert_true(errs.empty(), "test_builder_chained_adds_all_take_effect", LINE_DATA());
}

// ── ArgAmountValidatorBuilder::build — independência de instâncias ────────────

int test_builder_build_returns_independent_validator_copy()
{
	ArgAmountValidatorBuilder builder;
	builder.add(ParserTokenType::PT_LISTEN, schema::unsigned_integer().between(1, 1).name("listen"));
	ArgAmountValidator v1 = builder.build();

	// Modificar o builder DEPOIS do primeiro build() não deveria afetar v1
	builder.add(ParserTokenType::PT_ROOT, schema::unsigned_integer().between(1, 1).name("root"));
	ArgAmountValidator v2 = builder.build();

	Directive root(make_name("root", ParserTokenType::PT_ROOT), make_values(1));
	v1.validate(root); // v1 não conhece PT_ROOT
	std::vector<std::string> errs1;
	v1.dumpErrorsOn(errs1);

	v2.validate(root); // v2 conhece PT_ROOT
	std::vector<std::string> errs2;
	v2.dumpErrorsOn(errs2);

	bool ok = !errs1.empty() && errs2.empty();
	return assert_true(ok, "test_builder_build_returns_independent_validator_copy", LINE_DATA());
}

// ── ArgAmountValidatorBuilder::defaultValidations / withDefaultValidations ───

int test_default_validations_covers_listen()
{
	ArgAmountValidator v = ArgAmountValidatorBuilder::defaultValidations();
	Directive listen(make_name("listen", ParserTokenType::PT_LISTEN), make_values(1));
	v.validate(listen);
	std::vector<std::string> errs;
	v.dumpErrorsOn(errs);
	return assert_true(errs.empty(), "test_default_validations_covers_listen", LINE_DATA());
}

int test_default_validations_listen_requires_at_least_one_arg()
{
	ArgAmountValidator v = ArgAmountValidatorBuilder::defaultValidations();
	Directive listen(make_name("listen", ParserTokenType::PT_LISTEN), make_values(0));
	v.validate(listen);
	std::vector<std::string> errs;
	v.dumpErrorsOn(errs);
	return assert_true(errs.size() == 1, "test_default_validations_listen_requires_at_least_one_arg", LINE_DATA());
}

int test_default_validations_http_requires_zero_args()
{
	ArgAmountValidator v = ArgAmountValidatorBuilder::defaultValidations();
	Block http(make_name("http", ParserTokenType::PT_HTTP), make_values(0));
	v.validate(http);
	std::vector<std::string> errs;
	v.dumpErrorsOn(errs);
	return assert_true(errs.empty(), "test_default_validations_http_requires_zero_args", LINE_DATA());
}

int test_default_validations_http_rejects_any_arg()
{
	ArgAmountValidator v = ArgAmountValidatorBuilder::defaultValidations();
	Block http(make_name("http", ParserTokenType::PT_HTTP), make_values(1));
	v.validate(http);
	std::vector<std::string> errs;
	v.dumpErrorsOn(errs);
	return assert_true(errs.size() == 1, "test_default_validations_http_rejects_any_arg", LINE_DATA());
}

int test_default_validations_location_accepts_one_or_two_args()
{
	ArgAmountValidator v = ArgAmountValidatorBuilder::defaultValidations();
	Block loc1(make_name("location", ParserTokenType::PT_LOCATION), make_values(1));
	Block loc2(make_name("location", ParserTokenType::PT_LOCATION), make_values(2));
	v.validate(loc1);
	v.validate(loc2);
	std::vector<std::string> errs;
	v.dumpErrorsOn(errs);
	return assert_true(errs.empty(), "test_default_validations_location_accepts_one_or_two_args", LINE_DATA());
}

int test_default_validations_location_rejects_three_args()
{
	ArgAmountValidator v = ArgAmountValidatorBuilder::defaultValidations();
	Block loc(make_name("location", ParserTokenType::PT_LOCATION), make_values(3));
	v.validate(loc);
	std::vector<std::string> errs;
	v.dumpErrorsOn(errs);
	return assert_true(errs.size() == 1, "test_default_validations_location_rejects_three_args", LINE_DATA());
}

int test_default_validations_index_accepts_unlimited_args()
{
	ArgAmountValidator v = ArgAmountValidatorBuilder::defaultValidations();
	Directive index(make_name("index", ParserTokenType::PT_INDEX), make_values(20));
	v.validate(index);
	std::vector<std::string> errs;
	v.dumpErrorsOn(errs);
	return assert_true(errs.empty(), "test_default_validations_index_accepts_unlimited_args", LINE_DATA());
}

int test_default_validations_try_files_requires_at_least_two()
{
	ArgAmountValidator v = ArgAmountValidatorBuilder::defaultValidations();
	Directive ok_dir(make_name("try_files", ParserTokenType::PT_TRY_FILES), make_values(2));
	Directive bad_dir(make_name("try_files", ParserTokenType::PT_TRY_FILES), make_values(1));
	v.validate(ok_dir);
	v.validate(bad_dir);
	std::vector<std::string> errs;
	v.dumpErrorsOn(errs);
	return assert_true(errs.size() == 1, "test_default_validations_try_files_requires_at_least_two", LINE_DATA());
}

int test_default_validations_proxy_set_header_requires_exactly_two()
{
	ArgAmountValidator v = ArgAmountValidatorBuilder::defaultValidations();
	Directive ok_dir(make_name("proxy_set_header", ParserTokenType::PT_PROXY_SET_HEADER), make_values(2));
	Directive bad_dir(make_name("proxy_set_header", ParserTokenType::PT_PROXY_SET_HEADER), make_values(3));
	v.validate(ok_dir);
	v.validate(bad_dir);
	std::vector<std::string> errs;
	v.dumpErrorsOn(errs);
	return assert_true(errs.size() == 1, "test_default_validations_proxy_set_header_requires_exactly_two", LINE_DATA());
}

int test_default_validations_add_header_accepts_two_or_three()
{
	ArgAmountValidator v = ArgAmountValidatorBuilder::defaultValidations();
	Directive two(make_name("add_header", ParserTokenType::PT_ADD_HEADER), make_values(2));
	Directive three(make_name("add_header", ParserTokenType::PT_ADD_HEADER), make_values(3));
	Directive four(make_name("add_header", ParserTokenType::PT_ADD_HEADER), make_values(4));
	v.validate(two);
	v.validate(three);
	v.validate(four);
	std::vector<std::string> errs;
	v.dumpErrorsOn(errs);
	return assert_true(errs.size() == 1, "test_default_validations_add_header_accepts_two_or_three", LINE_DATA());
}

// Feature: PT_MAIN não é registrado por withDefaultValidations() — isso é
// esperado porque ParserVisitorValidator::visit(Block&) explicitamente pula
// a validação de argumentos quando block.name == PT_MAIN. Documentamos esse
// comportamento aqui no nível do ArgAmountValidator puro (sem o visitor).
int test_default_validations_does_not_cover_pt_main()
{
	ArgAmountValidator v = ArgAmountValidatorBuilder::defaultValidations();
	Block main_block(make_name("root", ParserTokenType::PT_MAIN), make_values(0));
	v.validate(main_block);
	std::vector<std::string> errs;
	v.dumpErrorsOn(errs);
	bool ok = errs.size() == 1
		&& errs[0].find("not found on arg_amount_validations") != std::string::npos;
	return assert_true(ok, "test_default_validations_does_not_cover_pt_main", LINE_DATA());
}

// ── main ──────────────────────────────────────────────────────────────────────

int main(int argc, char **argv)
{

	messages::settup(argc, argv);

	int failures = 0;

	// std::cout << "\n=== validate() — tipo não registrado ===\n";
	failures += test_validate_unregistered_type_generates_error();
	failures += test_validate_unregistered_type_mentions_type_name();

	// std::cout << "\n=== validate() — dentro do range ===\n";
	failures += test_validate_exact_count_within_range_passes();
	failures += test_validate_min_boundary_passes();
	failures += test_validate_max_boundary_passes();
	failures += test_validate_zero_args_with_between_zero_zero_passes();

	// std::cout << "\n=== validate() — fora do range ===\n";
	failures += test_validate_too_few_args_generates_error();
	failures += test_validate_too_many_args_generates_error();
	failures += test_validate_zero_args_with_between_one_one_fails();
	failures += test_validate_one_arg_with_between_zero_zero_fails();

	// std::cout << "\n=== validate() — aceita Block além de Directive ===\n";
	failures += test_validate_accepts_block_node();
	failures += test_validate_block_out_of_range_generates_error();

	// std::cout << "\n=== dumpErrorsOn() ===\n";
	failures += test_dumpErrorsOn_appends_to_destination();
	failures += test_dumpErrorsOn_clears_internal_list();
	failures += test_dumpErrorsOn_with_no_errors_does_not_modify_destination();

	// std::cout << "\n=== addValidation() — sobrescrita ===\n";
	failures += test_addValidation_overwrites_previous_rule_for_same_type();

	// std::cout << "\n=== ArgAmountValidatorBuilder::add — fluent chaining ===\n";
	failures += test_builder_add_returns_self_for_chaining();
	failures += test_builder_chained_adds_all_take_effect();

	// std::cout << "\n=== ArgAmountValidatorBuilder::build — independência ===\n";
	failures += test_builder_build_returns_independent_validator_copy();

	// std::cout << "\n=== defaultValidations() / withDefaultValidations() ===\n";
	failures += test_default_validations_covers_listen();
	failures += test_default_validations_listen_requires_at_least_one_arg();
	failures += test_default_validations_http_requires_zero_args();
	failures += test_default_validations_http_rejects_any_arg();
	failures += test_default_validations_location_accepts_one_or_two_args();
	failures += test_default_validations_location_rejects_three_args();
	failures += test_default_validations_index_accepts_unlimited_args();
	failures += test_default_validations_try_files_requires_at_least_two();
	failures += test_default_validations_proxy_set_header_requires_exactly_two();
	failures += test_default_validations_add_header_accepts_two_or_three();
	failures += test_default_validations_does_not_cover_pt_main();

	messages::print();

	return failures;
}
