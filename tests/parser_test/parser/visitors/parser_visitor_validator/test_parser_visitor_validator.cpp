/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_parser_visitor_validator.cpp                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/21 11:06:23 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/21 11:06:25 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "test_utils.hpp"
#include "ParserVisitorValidator.hpp"

#include <vector>

// ── helpers ──────────────────────────────────────────────────────────────────

static ParserToken make_name(const std::string &content, ParserTokenType::type type,
							size_t line = 1, size_t col = 0)
{
	return ParserToken("test.conf", line, col, content, type);
}

static std::vector<ParserToken> make_values(size_t n)
{
	std::vector<ParserToken> values;
	for (size_t i = 0; i < n; i++)
		values.push_back(ParserToken("test.conf", 1, 0, "v", ParserTokenType::PT_WORD));
	return values;
}

// ── visit(Directive&) — quantidade de argumentos ──────────────────────────────

int test_visit_directive_with_valid_arg_count_no_error()
{
	ParserVisitorValidator validator;
	Directive listen(make_name("listen", ParserTokenType::PT_LISTEN), make_values(1));
	listen.accept(validator);
	return assert_true(validator.getErrors().empty(),
					"test_visit_directive_with_valid_arg_count_no_error", LINE_DATA());
}

int test_visit_directive_with_invalid_arg_count_generates_error()
{
	ParserVisitorValidator validator;
	Directive listen(make_name("listen", ParserTokenType::PT_LISTEN)); // 0 args, exige >=1
	listen.accept(validator);
	return assert_true(validator.getErrors().size() == 1,
					"test_visit_directive_with_invalid_arg_count_generates_error", LINE_DATA());
}

// ── visit(Directive&) — PT_END e PT_WORD são ignorados ────────────────────────

int test_visit_directive_pt_end_is_ignored()
{
	ParserVisitorValidator validator;
	Directive end_node(make_name("", ParserTokenType::PT_END));
	end_node.accept(validator);
	return assert_true(validator.getErrors().empty(), "test_visit_directive_pt_end_is_ignored", LINE_DATA());
}

int test_visit_directive_pt_word_is_ignored()
{
	ParserVisitorValidator validator;
	Directive word_node(make_name("unknown_thing", ParserTokenType::PT_WORD));
	word_node.accept(validator);
	return assert_true(validator.getErrors().empty(), "test_visit_directive_pt_word_is_ignored", LINE_DATA());
}

// ── visit(Block&) — quantidade de argumentos do próprio bloco ────────────────

int test_visit_block_with_valid_arg_count_no_arg_error()
{
	ParserVisitorValidator validator;
	Block http(make_name("http", ParserTokenType::PT_HTTP)); // 0 args, regra default exige [0,0]
	http.accept(validator);
	return assert_true(validator.getErrors().empty(),
					"test_visit_block_with_valid_arg_count_no_arg_error", LINE_DATA());
}

int test_visit_block_with_invalid_arg_count_generates_error()
{
	ParserVisitorValidator validator;
	Block http(make_name("http", ParserTokenType::PT_HTTP), make_values(1)); // deveria ser 0
	http.accept(validator);
	bool ok = false;
	for (size_t i = 0; i < validator.getErrors().size(); i++)
		if (validator.getErrors()[i].find("http args amount") != std::string::npos)
			ok = true;
	return assert_true(ok, "test_visit_block_with_invalid_arg_count_generates_error", LINE_DATA());
}

// ── visit(Block&) — PT_MAIN nunca é validado quanto à quantidade de args ────

int test_visit_block_pt_main_skips_arg_amount_validation()
{
	ParserVisitorValidator validator;
	// PT_MAIN não está nas validações default; se fosse validado, geraria
	// o erro "Type PT_MAIN not found on arg_amount_validations." Como o
	// visitor pula explicitamente esse caso, não deve haver erro algum.
	Block root(make_name("root", ParserTokenType::PT_MAIN));
	root.accept(validator);
	return assert_true(validator.getErrors().empty(),
					"test_visit_block_pt_main_skips_arg_amount_validation", LINE_DATA());
}

// ── visit(Block&) — PT_END e PT_WORD como bloco são ignorados ────────────────

int test_visit_block_pt_end_is_ignored()
{
	ParserVisitorValidator validator;
	Block end_block(make_name("", ParserTokenType::PT_END));
	end_block.accept(validator);
	return assert_true(validator.getErrors().empty(), "test_visit_block_pt_end_is_ignored", LINE_DATA());
}

int test_visit_block_pt_word_is_ignored()
{
	ParserVisitorValidator validator;
	Block word_block(make_name("mystery", ParserTokenType::PT_WORD));
	word_block.accept(validator);
	return assert_true(validator.getErrors().empty(), "test_visit_block_pt_word_is_ignored", LINE_DATA());
}

// ── visit(Block&) — validação de escopo dos filhos ────────────────────────────

int test_visit_block_validates_children_scope()
{
	ParserVisitorValidator validator;
	// server diretamente dentro de PT_MAIN (sem http) -> escopo errado pela
	// regra padrão (PT_SERVER só é permitido em PT_HTTP)
	Block *root = new Block(make_name("root", ParserTokenType::PT_MAIN));
	Block *server = new Block(make_name("server", ParserTokenType::PT_SERVER));
	root->children.push_back(server);

	root->accept(validator);
	bool ok = false;
	for (size_t i = 0; i < validator.getErrors().size(); i++)
		if (validator.getErrors()[i].find("in wrong context") != std::string::npos)
			ok = true;
	delete root;
	return assert_true(ok, "test_visit_block_validates_children_scope", LINE_DATA());
}

int test_visit_block_children_in_correct_scope_no_scope_error()
{
	ParserVisitorValidator validator;
	Block *root = new Block(make_name("root", ParserTokenType::PT_MAIN));
	Block *http = new Block(make_name("http", ParserTokenType::PT_HTTP));
	Block *server = new Block(make_name("server", ParserTokenType::PT_SERVER));
	http->children.push_back(server);
	root->children.push_back(http);

	root->accept(validator);
	bool has_scope_error = false;
	for (size_t i = 0; i < validator.getErrors().size(); i++)
		if (validator.getErrors()[i].find("wrong context") != std::string::npos
		|| validator.getErrors()[i].find("scope not found") != std::string::npos)
			has_scope_error = true;
	delete root;
	return assert_true(!has_scope_error,
					"test_visit_block_children_in_correct_scope_no_scope_error", LINE_DATA());
}

// ── visit(Block&) — percurso recursivo completo ───────────────────────────────

int test_visit_recurses_into_nested_blocks()
{
	ParserVisitorValidator validator;
	Block *root = new Block(make_name("root", ParserTokenType::PT_MAIN));
	Block *http = new Block(make_name("http", ParserTokenType::PT_HTTP));
	Block *server = new Block(make_name("server", ParserTokenType::PT_SERVER));
	// listen com 0 args dentro de server -> deve ser detectado mesmo a 3
	// níveis de profundidade da raiz
	Directive *listen = new Directive(make_name("listen", ParserTokenType::PT_LISTEN));

	server->children.push_back(listen);
	http->children.push_back(server);
	root->children.push_back(http);

	root->accept(validator);
	bool ok = false;
	for (size_t i = 0; i < validator.getErrors().size(); i++)
		if (validator.getErrors()[i].find("listen args amount") != std::string::npos)
			ok = true;
	delete root;
	return assert_true(ok, "test_visit_recurses_into_nested_blocks", LINE_DATA());
}

int test_visit_collects_errors_from_multiple_branches()
{
	ParserVisitorValidator validator;
	Block *root = new Block(make_name("root", ParserTokenType::PT_MAIN));
	Block *http = new Block(make_name("http", ParserTokenType::PT_HTTP));

	Block *server1 = new Block(make_name("server", ParserTokenType::PT_SERVER));
	Directive *listen1 = new Directive(make_name("listen", ParserTokenType::PT_LISTEN)); // erro: 0 args
	server1->children.push_back(listen1);

	Block *server2 = new Block(make_name("server", ParserTokenType::PT_SERVER));
	Directive *root_dir2 = new Directive(make_name("root", ParserTokenType::PT_ROOT), make_values(2)); // erro: deveria ser 1
	server2->children.push_back(root_dir2);

	http->children.push_back(server1);
	http->children.push_back(server2);
	root->children.push_back(http);

	root->accept(validator);
	// Esperamos pelo menos 2 erros (um de cada ramo)
	bool ok = validator.getErrors().size() >= 2;
	delete root;
	return assert_true(ok, "test_visit_collects_errors_from_multiple_branches", LINE_DATA());
}

// ── pushScope/popScope — equilíbrio correto via accept() recursivo ───────────

int test_scope_pop_restores_sibling_validation_correctly()
{
	// Depois de processar um bloco "server" (que empilha/desempilha PT_SERVER),
	// um segundo "server" irmão deve ser validado contra o MESMO escopo do
	// pai (PT_HTTP), não contra o escopo deixado pelo primeiro server.
	ParserVisitorValidator validator;
	Block *root = new Block(make_name("root", ParserTokenType::PT_MAIN));
	Block *http = new Block(make_name("http", ParserTokenType::PT_HTTP));

	Block *server1 = new Block(make_name("server", ParserTokenType::PT_SERVER));
	Block *loc1 = new Block(make_name("location", ParserTokenType::PT_LOCATION), make_values(1));
	server1->children.push_back(loc1); // location dentro de server: válido

	Block *server2 = new Block(make_name("server", ParserTokenType::PT_SERVER));
	// server2 também deve ser validado corretamente como filho de http,
	// mesmo depois de toda a recursão de server1/loc1 ter sido desempilhada.
	Directive *listen2 = new Directive(make_name("listen", ParserTokenType::PT_LISTEN), make_values(1));
	server2->children.push_back(listen2);

	http->children.push_back(server1);
	http->children.push_back(server2);
	root->children.push_back(http);

	root->accept(validator);
	bool has_unexpected_error = !validator.getErrors().empty();
	delete root;
	return assert_true(!has_unexpected_error,
					"test_scope_pop_restores_sibling_validation_correctly", LINE_DATA());
}

// ── Construtor com builders customizados ──────────────────────────────────────

int test_custom_builders_constructor_uses_custom_rules()
{
	ScopeValidatorBuilder scope_builder;
	scope_builder.withBlockOnScope(ParserTokenType::PT_SERVER, ParserTokenType::PT_MAIN); // regra NÃO-padrão

	ArgAmountValidatorBuilder arg_builder;
	arg_builder.add(ParserTokenType::PT_SERVER, schema::unsigned_integer().between(0, 0).name("server"));

	ParserVisitorValidator validator(scope_builder, arg_builder);

	// Com a regra customizada, server DIRETO em PT_MAIN (sem http) deve ser válido
	Block *root = new Block(make_name("root", ParserTokenType::PT_MAIN));
	Block *server = new Block(make_name("server", ParserTokenType::PT_SERVER));
	root->children.push_back(server);

	root->accept(validator);
	bool ok = validator.getErrors().empty();
	delete root;
	return assert_true(ok, "test_custom_builders_constructor_uses_custom_rules", LINE_DATA());
}

int test_custom_builders_constructor_only_knows_registered_types()
{
	// Builders customizados vazios (sem withDefault*) -> qualquer tipo não
	// registrado explicitamente deve gerar "scope not found" / "not found
	// on arg_amount_validations", mesmo para tipos comuns como PT_LISTEN
	// que teriam regras no conjunto padrão.
	ScopeValidatorBuilder empty_scope_builder;
	ArgAmountValidatorBuilder empty_arg_builder;
	ParserVisitorValidator validator(empty_scope_builder, empty_arg_builder);

	Block *root = new Block(make_name("root", ParserTokenType::PT_MAIN));
	Block *http = new Block(make_name("http", ParserTokenType::PT_HTTP));
	root->children.push_back(http);

	root->accept(validator);
	bool ok = false;
	for (size_t i = 0; i < validator.getErrors().size(); i++)
		if (validator.getErrors()[i].find("scope not found") != std::string::npos)
			ok = true;
	delete root;
	return assert_true(ok, "test_custom_builders_constructor_only_knows_registered_types", LINE_DATA());
}

// ── getErrors() — acessor ─────────────────────────────────────────────────────

int test_getErrors_starts_empty()
{
	ParserVisitorValidator validator;
	return assert_true(validator.getErrors().empty(), "test_getErrors_starts_empty", LINE_DATA());
}

int test_getErrors_accumulates_across_multiple_accept_calls()
{
	ParserVisitorValidator validator;
	Directive bad1(make_name("listen", ParserTokenType::PT_LISTEN)); // 0 args -> erro
	Directive bad2(make_name("root", ParserTokenType::PT_ROOT), make_values(2)); // 2 args -> erro (deveria ser 1)

	bad1.accept(validator);
	bad2.accept(validator);

	return assert_true(validator.getErrors().size() == 2,
					"test_getErrors_accumulates_across_multiple_accept_calls", LINE_DATA());
}

// ── Configuração completa via defaults (caso real de uso) ────────────────────

int test_full_realistic_config_with_no_errors()
{
	// server { listen 80; root /var/www; location /img { root /imgs; } }
	// dentro de http { ... }
	ParserVisitorValidator validator; // construtor default usa todas as regras padrão

	Block *root = new Block(make_name("root", ParserTokenType::PT_MAIN));
	Block *http = new Block(make_name("http", ParserTokenType::PT_HTTP));
	Block *server = new Block(make_name("server", ParserTokenType::PT_SERVER));

	Directive *listen = new Directive(make_name("listen", ParserTokenType::PT_LISTEN), make_values(1));
	Directive *root_dir = new Directive(make_name("root", ParserTokenType::PT_ROOT), make_values(1));

	std::vector<ParserToken> loc_vals; loc_vals.push_back(make_name("/img", ParserTokenType::PT_WORD));
	Block *location = new Block(make_name("location", ParserTokenType::PT_LOCATION), loc_vals);
	Directive *loc_root = new Directive(make_name("root", ParserTokenType::PT_ROOT), make_values(1));
	location->children.push_back(loc_root);

	server->children.push_back(listen);
	server->children.push_back(root_dir);
	server->children.push_back(location);
	http->children.push_back(server);
	root->children.push_back(http);

	root->accept(validator);
	bool ok = validator.getErrors().empty();
	if (!ok)
		for (size_t i = 0; i < validator.getErrors().size(); i++)
			// std::cout << "    unexpected: " << validator.getErrors()[i] << "\n";
	delete root;
	return assert_true(ok, "test_full_realistic_config_with_no_errors", LINE_DATA());
}

int test_full_realistic_config_with_multiple_kinds_of_errors()
{
	ParserVisitorValidator validator;

	Block *root = new Block(make_name("root", ParserTokenType::PT_MAIN));
	// server fora de http -> erro de escopo
	Block *server = new Block(make_name("server", ParserTokenType::PT_SERVER));
	// listen sem argumentos -> erro de quantidade
	Directive *listen = new Directive(make_name("listen", ParserTokenType::PT_LISTEN));
	server->children.push_back(listen);
	root->children.push_back(server);

	root->accept(validator);
	bool has_scope_error = false, has_amount_error = false;
	for (size_t i = 0; i < validator.getErrors().size(); i++)
	{
		if (validator.getErrors()[i].find("wrong context") != std::string::npos)
			has_scope_error = true;
		if (validator.getErrors()[i].find("listen args amount") != std::string::npos)
			has_amount_error = true;
	}
	bool ok = has_scope_error && has_amount_error;
	delete root;
	return assert_true(ok, "test_full_realistic_config_with_multiple_kinds_of_errors", LINE_DATA());
}

// ── main ──────────────────────────────────────────────────────────────────────

int main(int argc, char **argv)
{

	messages::settup(argc, argv);

	int failures = 0;

	// std::cout << "\n=== visit(Directive&) — quantidade de argumentos ===\n";
	failures += test_visit_directive_with_valid_arg_count_no_error();
	failures += test_visit_directive_with_invalid_arg_count_generates_error();

	// std::cout << "\n=== visit(Directive&) — PT_END / PT_WORD ignorados ===\n";
	failures += test_visit_directive_pt_end_is_ignored();
	failures += test_visit_directive_pt_word_is_ignored();

	// std::cout << "\n=== visit(Block&) — quantidade de argumentos do próprio bloco ===\n";
	failures += test_visit_block_with_valid_arg_count_no_arg_error();
	failures += test_visit_block_with_invalid_arg_count_generates_error();

	// std::cout << "\n=== visit(Block&) — PT_MAIN pula validação de quantidade ===\n";
	failures += test_visit_block_pt_main_skips_arg_amount_validation();

	// std::cout << "\n=== visit(Block&) — PT_END / PT_WORD ignorados ===\n";
	failures += test_visit_block_pt_end_is_ignored();
	failures += test_visit_block_pt_word_is_ignored();

	// std::cout << "\n=== visit(Block&) — validação de escopo dos filhos ===\n";
	failures += test_visit_block_validates_children_scope();
	failures += test_visit_block_children_in_correct_scope_no_scope_error();

	// std::cout << "\n=== visit(Block&) — percurso recursivo completo ===\n";
	failures += test_visit_recurses_into_nested_blocks();
	failures += test_visit_collects_errors_from_multiple_branches();

	// std::cout << "\n=== pushScope/popScope — equilíbrio entre irmãos ===\n";
	failures += test_scope_pop_restores_sibling_validation_correctly();

	// std::cout << "\n=== Construtor com builders customizados ===\n";
	failures += test_custom_builders_constructor_uses_custom_rules();
	failures += test_custom_builders_constructor_only_knows_registered_types();

	// std::cout << "\n=== getErrors() ===\n";
	failures += test_getErrors_starts_empty();
	failures += test_getErrors_accumulates_across_multiple_accept_calls();

	// std::cout << "\n=== Configuração completa via defaults ===\n";
	failures += test_full_realistic_config_with_no_errors();
	failures += test_full_realistic_config_with_multiple_kinds_of_errors();

	messages::print();

	return failures;
}
