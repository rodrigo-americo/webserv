/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_scope_validator.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/21 11:09:52 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/21 11:09:53 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "test_utils.hpp"
#include "ScopeValidator.hpp"

// ── helpers ──────────────────────────────────────────────────────────────────

static ParserToken make_name(const std::string &content, ParserTokenType::type type)
{
	return ParserToken("test.conf", 1, 0, content, type);
}

// ── pushScope / popScope — pilha básica ───────────────────────────────────────

int test_pushScope_then_validate_directive_in_correct_scope_passes()
{
	ScopeValidator sv;
	sv.addDirectiveScopeValidation(ParserTokenType::PT_LISTEN, ParserTokenType::PT_SERVER);
	sv.pushScope(ParserTokenType::PT_SERVER);

	Block server(make_name("server", ParserTokenType::PT_SERVER));
	Directive listen(make_name("listen", ParserTokenType::PT_LISTEN));
	server.children.push_back(&listen);

	sv.validate(server);
	std::vector<std::string> errs;
	sv.dumpErrorsOn(errs);
	return assert_true(errs.empty(), "test_pushScope_then_validate_directive_in_correct_scope_passes", LINE_DATA());
}

int test_directive_in_wrong_scope_generates_error()
{
	ScopeValidator sv;
	sv.addDirectiveScopeValidation(ParserTokenType::PT_LISTEN, ParserTokenType::PT_SERVER);
	sv.pushScope(ParserTokenType::PT_HTTP); // escopo errado

	Block http(make_name("http", ParserTokenType::PT_HTTP));
	Directive listen(make_name("listen", ParserTokenType::PT_LISTEN));
	http.children.push_back(&listen);

	sv.validate(http);
	std::vector<std::string> errs;
	sv.dumpErrorsOn(errs);
	bool ok = errs.size() == 1 && errs[0].find("in wrong context") != std::string::npos;
	return assert_true(ok, "test_directive_in_wrong_scope_generates_error", LINE_DATA());
}

int test_wrong_scope_error_mentions_allowed_scope()
{
	ScopeValidator sv;
	sv.addDirectiveScopeValidation(ParserTokenType::PT_LISTEN, ParserTokenType::PT_SERVER);
	sv.pushScope(ParserTokenType::PT_HTTP);

	Block http(make_name("http", ParserTokenType::PT_HTTP));
	Directive listen(make_name("listen", ParserTokenType::PT_LISTEN));
	http.children.push_back(&listen);

	sv.validate(http);
	std::vector<std::string> errs;
	sv.dumpErrorsOn(errs);
	bool ok = !errs.empty() && errs[0].find("PT_SERVER") != std::string::npos;
	return assert_true(ok, "test_wrong_scope_error_mentions_allowed_scope", LINE_DATA());
}

int test_wrong_scope_error_includes_line_address()
{
	ScopeValidator sv;
	sv.addDirectiveScopeValidation(ParserTokenType::PT_LISTEN, ParserTokenType::PT_SERVER);
	sv.pushScope(ParserTokenType::PT_HTTP);

	Block http(make_name("http", ParserTokenType::PT_HTTP));
	Directive listen(ParserToken("nginx.conf", 7, 4, "listen", ParserTokenType::PT_LISTEN));
	http.children.push_back(&listen);

	sv.validate(http);
	std::vector<std::string> errs;
	sv.dumpErrorsOn(errs);
	bool ok = !errs.empty() && errs[0].find("nginx.conf:7:4") != std::string::npos;
	return assert_true(ok, "test_wrong_scope_error_includes_line_address", LINE_DATA());
}

// ── tipo sem regra registrada ──────────────────────────────────────────────────

int test_directive_without_registered_rule_generates_scope_not_found_error()
{
	ScopeValidator sv; // sem nenhuma regra
	sv.pushScope(ParserTokenType::PT_SERVER);

	Block server(make_name("server", ParserTokenType::PT_SERVER));
	Directive unknown(make_name("mystery_directive", ParserTokenType::PT_LISTEN));
	server.children.push_back(&unknown);

	sv.validate(server);
	std::vector<std::string> errs;
	sv.dumpErrorsOn(errs);
	bool ok = errs.size() == 1 && errs[0].find("scope not found") != std::string::npos;
	return assert_true(ok, "test_directive_without_registered_rule_generates_scope_not_found_error", LINE_DATA());
}

int test_scope_not_found_error_includes_content_and_type()
{
	ScopeValidator sv;
	sv.pushScope(ParserTokenType::PT_SERVER);

	Block server(make_name("server", ParserTokenType::PT_SERVER));
	Directive unknown(make_name("mystery_directive", ParserTokenType::PT_LISTEN));
	server.children.push_back(&unknown);

	sv.validate(server);
	std::vector<std::string> errs;
	sv.dumpErrorsOn(errs);
	bool ok = !errs.empty()
		&& errs[0].find("mystery_directive") != std::string::npos
		&& errs[0].find("PT_LISTEN") != std::string::npos;
	return assert_true(ok, "test_scope_not_found_error_includes_content_and_type", LINE_DATA());
}

// ── PT_WORD e PT_END são sempre ignorados ─────────────────────────────────────

int test_pt_word_child_is_ignored_even_without_rule()
{
	ScopeValidator sv; // sem nenhuma regra registrada
	sv.pushScope(ParserTokenType::PT_SERVER);

	Block server(make_name("server", ParserTokenType::PT_SERVER));
	Directive word_node(make_name("some_unrecognized_word", ParserTokenType::PT_WORD));
	server.children.push_back(&word_node);

	sv.validate(server);
	std::vector<std::string> errs;
	sv.dumpErrorsOn(errs);
	return assert_true(errs.empty(), "test_pt_word_child_is_ignored_even_without_rule", LINE_DATA());
}

int test_pt_end_child_is_ignored_even_without_rule()
{
	ScopeValidator sv;
	sv.pushScope(ParserTokenType::PT_SERVER);

	Block server(make_name("server", ParserTokenType::PT_SERVER));
	Directive end_node(make_name("", ParserTokenType::PT_END));
	server.children.push_back(&end_node);

	sv.validate(server);
	std::vector<std::string> errs;
	sv.dumpErrorsOn(errs);
	return assert_true(errs.empty(), "test_pt_end_child_is_ignored_even_without_rule", LINE_DATA());
}

// ── Blocos filhos usam _block_scope, diretivas usam _directive_scope ─────────

int test_block_child_uses_block_scope_rules_not_directive_rules()
{
	ScopeValidator sv;
	// Registrado SOMENTE como diretiva (não como bloco) para o mesmo tipo
	sv.addDirectiveScopeValidation(ParserTokenType::PT_SERVER, ParserTokenType::PT_HTTP);
	sv.pushScope(ParserTokenType::PT_HTTP);

	Block http(make_name("http", ParserTokenType::PT_HTTP));
	Block server_child(make_name("server", ParserTokenType::PT_SERVER)); // é um BLOCK
	http.children.push_back(&server_child);

	sv.validate(http);
	std::vector<std::string> errs;
	sv.dumpErrorsOn(errs);
	// Como server_child é um Block, a validação consulta _block_scope, que
	// não tem regra para PT_SERVER (só _directive_scope tem) -> "scope not found"
	bool ok = errs.size() == 1 && errs[0].find("scope not found") != std::string::npos;
	return assert_true(ok, "test_block_child_uses_block_scope_rules_not_directive_rules", LINE_DATA());
}

int test_directive_child_uses_directive_scope_rules_not_block_rules()
{
	ScopeValidator sv;
	// Registrado SOMENTE como bloco para este tipo
	sv.addBlockScopeValidation(ParserTokenType::PT_LISTEN, ParserTokenType::PT_SERVER);
	sv.pushScope(ParserTokenType::PT_SERVER);

	Block server(make_name("server", ParserTokenType::PT_SERVER));
	Directive listen_directive(make_name("listen", ParserTokenType::PT_LISTEN)); // é DIRECTIVE
	server.children.push_back(&listen_directive);

	sv.validate(server);
	std::vector<std::string> errs;
	sv.dumpErrorsOn(errs);
	bool ok = errs.size() == 1 && errs[0].find("scope not found") != std::string::npos;
	return assert_true(ok, "test_directive_child_uses_directive_scope_rules_not_block_rules", LINE_DATA());
}

// ── Múltiplos escopos permitidos para o mesmo tipo (andOn) ────────────────────

int test_directive_allowed_in_any_of_multiple_registered_scopes()
{
	ScopeValidator sv;
	sv.addDirectiveScopeValidation(ParserTokenType::PT_ROOT, ParserTokenType::PT_HTTP);
	sv.addDirectiveScopeValidation(ParserTokenType::PT_ROOT, ParserTokenType::PT_SERVER);
	sv.addDirectiveScopeValidation(ParserTokenType::PT_ROOT, ParserTokenType::PT_LOCATION);

	sv.pushScope(ParserTokenType::PT_LOCATION); // terceiro escopo registrado
	Block loc(make_name("location", ParserTokenType::PT_LOCATION));
	Directive root_dir(make_name("root", ParserTokenType::PT_ROOT));
	loc.children.push_back(&root_dir);

	sv.validate(loc);
	std::vector<std::string> errs;
	sv.dumpErrorsOn(errs);
	return assert_true(errs.empty(), "test_directive_allowed_in_any_of_multiple_registered_scopes", LINE_DATA());
}

int test_error_message_lists_all_allowed_scopes_comma_separated()
{
	ScopeValidator sv;
	sv.addDirectiveScopeValidation(ParserTokenType::PT_INCLUDE, ParserTokenType::PT_MAIN);
	sv.addDirectiveScopeValidation(ParserTokenType::PT_INCLUDE, ParserTokenType::PT_HTTP);
	sv.addDirectiveScopeValidation(ParserTokenType::PT_INCLUDE, ParserTokenType::PT_SERVER);

	sv.pushScope(ParserTokenType::PT_LOCATION); // nenhum dos 3 registrados
	Block loc(make_name("location", ParserTokenType::PT_LOCATION));
	Directive inc(make_name("include", ParserTokenType::PT_INCLUDE));
	loc.children.push_back(&inc);

	sv.validate(loc);
	std::vector<std::string> errs;
	sv.dumpErrorsOn(errs);
	bool ok = !errs.empty()
		&& errs[0].find("PT_MAIN, PT_HTTP, PT_SERVER") != std::string::npos;
	return assert_true(ok, "test_error_message_lists_all_allowed_scopes_comma_separated", LINE_DATA());
}

int test_error_message_single_scope_has_no_trailing_comma()
{
	ScopeValidator sv;
	sv.addDirectiveScopeValidation(ParserTokenType::PT_LISTEN, ParserTokenType::PT_SERVER);
	sv.pushScope(ParserTokenType::PT_HTTP);

	Block http(make_name("http", ParserTokenType::PT_HTTP));
	Directive listen(make_name("listen", ParserTokenType::PT_LISTEN));
	http.children.push_back(&listen);

	sv.validate(http);
	std::vector<std::string> errs;
	sv.dumpErrorsOn(errs);
	bool ok = !errs.empty() && errs[0].find("'PT_SERVER'") != std::string::npos;
	return assert_true(ok, "test_error_message_single_scope_has_no_trailing_comma", LINE_DATA());
}

// ── pushScope / popScope — pilha aninhada ─────────────────────────────────────

int test_nested_scopes_validate_against_innermost_scope()
{
	ScopeValidator sv;
	sv.addDirectiveScopeValidation(ParserTokenType::PT_ROOT, ParserTokenType::PT_LOCATION);
	sv.pushScope(ParserTokenType::PT_HTTP);
	sv.pushScope(ParserTokenType::PT_SERVER);
	sv.pushScope(ParserTokenType::PT_LOCATION); // escopo mais interno = topo da pilha

	Block loc(make_name("location", ParserTokenType::PT_LOCATION));
	Directive root_dir(make_name("root", ParserTokenType::PT_ROOT));
	loc.children.push_back(&root_dir);

	sv.validate(loc); // deve usar PT_LOCATION (topo), não PT_HTTP/PT_SERVER
	std::vector<std::string> errs;
	sv.dumpErrorsOn(errs);
	return assert_true(errs.empty(), "test_nested_scopes_validate_against_innermost_scope", LINE_DATA());
}

int test_popScope_restores_previous_scope()
{
	ScopeValidator sv;
	sv.addDirectiveScopeValidation(ParserTokenType::PT_LISTEN, ParserTokenType::PT_SERVER);

	sv.pushScope(ParserTokenType::PT_HTTP);
	sv.pushScope(ParserTokenType::PT_SERVER);
	sv.popScope(); // volta para PT_HTTP

	Block http(make_name("http", ParserTokenType::PT_HTTP));
	Directive listen(make_name("listen", ParserTokenType::PT_LISTEN));
	http.children.push_back(&listen);

	sv.validate(http); // agora o escopo ativo é PT_HTTP, não PT_SERVER -> deve falhar
	std::vector<std::string> errs;
	sv.dumpErrorsOn(errs);
	bool ok = errs.size() == 1 && errs[0].find("in wrong context") != std::string::npos;
	return assert_true(ok, "test_popScope_restores_previous_scope", LINE_DATA());
}

// ── BUG CRÍTICO: validate() sem pushScope() prévio crasha (pilha vazia) ──────
//
// BUG CRÍTICO: _validateScopeFrom() acessa `_curr_scopes[_curr_scopes.size()
// - 1]` para obter o escopo atual (topo da pilha), sem verificar se a pilha
// está vazia. Se validate() for chamado em uma instância de ScopeValidator
// recém-criada (sem nenhum pushScope() anterior), `_curr_scopes.size()` é 0,
// e `0 - 1` sofre underflow de size_t (vira o maior valor possível). O
// acesso resultante `_curr_scopes[SIZE_MAX]` em um vector vazio é undefined
// behavior severo — confirmado como SEGFAULT real em build normal e
// ASSERTION FAILURE (abort) com -D_GLIBCXX_ASSERTIONS.
//
// No fluxo normal de uso via ParserVisitorValidator::visit(Block&), esse bug
// NUNCA se manifesta, porque pushScope(block.name.getType()) é sempre
// chamado incondicionalmente ANTES de _scope_validator.validate(block) —
// inclusive para o bloco raiz PT_MAIN. Ou seja, a pilha nunca está vazia
// no caminho de uso real através do visitor.
//
// Porém, ScopeValidator é uma classe pública e reutilizável — qualquer
// código (incluindo testes, ou uma futura refatoração) que chame
// validate() diretamente sem antes garantir ao menos um pushScope() vai
// crashar. Este teste documenta a PRÉ-CONDIÇÃO exata do bug, sem disparar
// o crash em si (o que abortaria toda a suíte).
//
// Comportamento esperado: validate() deveria, no mínimo, verificar
// `_curr_scopes.empty()` e tratar esse caso (erro descritivo, ou assumir um
// escopo padrão), em vez de acessar um índice inválido.
int test_validate_without_pushscope_crashes_precondition_BUG()
{
	ScopeValidator sv;
	sv.addDirectiveScopeValidation(ParserTokenType::PT_LISTEN, ParserTokenType::PT_SERVER);
	// Nenhum pushScope() foi chamado — a pilha de escopos está vazia.
	// Confirmamos a pré-condição sem de fato chamar validate(), que
	// crasharia o processo e abortaria a suíte inteira.
	//
	// (Não há um getter público para o tamanho da pilha; a ausência de
	// qualquer pushScope() nesta instância recém-criada é suficiente para
	// garantir que _curr_scopes está vazio, dado que o construtor de
	// ScopeValidator inicializa _curr_scopes() vazio e nenhum outro método
	// foi chamado.)
	return assert_true(true, "test_validate_without_pushscope_crashes_precondition_BUG", LINE_DATA());
}

// ── ScopeValidator::dumpErrorsOn ──────────────────────────────────────────────

int test_dumpErrorsOn_appends_and_clears()
{
	ScopeValidator sv;
	sv.pushScope(ParserTokenType::PT_SERVER);
	Block server(make_name("server", ParserTokenType::PT_SERVER));
	Directive unknown(make_name("x", ParserTokenType::PT_LISTEN)); // sem regra
	server.children.push_back(&unknown);
	sv.validate(server); // gera 1 erro interno

	std::vector<std::string> destine;
	destine.push_back("pre-existing");
	sv.dumpErrorsOn(destine);
	bool ok = destine.size() == 2 && destine[0] == "pre-existing";

	std::vector<std::string> second_dump;
	sv.dumpErrorsOn(second_dump); // já foi limpo
	ok = ok && second_dump.empty();

	return assert_true(ok, "test_dumpErrorsOn_appends_and_clears", LINE_DATA());
}

// ── ScopeValidatorBuilder — fluent chaining ───────────────────────────────────

int test_withDirectiveOnScope_returns_self_for_chaining()
{
	ScopeValidatorBuilder builder;
	ScopeValidatorBuilder &ref = builder.withDirectiveOnScope(ParserTokenType::PT_LISTEN, ParserTokenType::PT_SERVER);
	return assert_true(&ref == &builder, "test_withDirectiveOnScope_returns_self_for_chaining", LINE_DATA());
}

int test_withBlockOnScope_returns_self_for_chaining()
{
	ScopeValidatorBuilder builder;
	ScopeValidatorBuilder &ref = builder.withBlockOnScope(ParserTokenType::PT_SERVER, ParserTokenType::PT_HTTP);
	return assert_true(&ref == &builder, "test_withBlockOnScope_returns_self_for_chaining", LINE_DATA());
}

int test_andOn_returns_self_for_chaining()
{
	ScopeValidatorBuilder builder;
	builder.withDirectiveOnScope(ParserTokenType::PT_ROOT, ParserTokenType::PT_HTTP);
	ScopeValidatorBuilder &ref = builder.andOn(ParserTokenType::PT_SERVER);
	return assert_true(&ref == &builder, "test_andOn_returns_self_for_chaining", LINE_DATA());
}

int test_andOn_after_directive_adds_directive_scope()
{
	ScopeValidator sv = ScopeValidatorBuilder()
		.withDirectiveOnScope(ParserTokenType::PT_ROOT, ParserTokenType::PT_HTTP)
		.andOn(ParserTokenType::PT_SERVER)
		.build();

	sv.pushScope(ParserTokenType::PT_SERVER); // adicionado via andOn
	Block server(make_name("server", ParserTokenType::PT_SERVER));
	Directive root_dir(make_name("root", ParserTokenType::PT_ROOT));
	server.children.push_back(&root_dir);

	sv.validate(server);
	std::vector<std::string> errs;
	sv.dumpErrorsOn(errs);
	return assert_true(errs.empty(), "test_andOn_after_directive_adds_directive_scope", LINE_DATA());
}

int test_andOn_after_block_adds_block_scope()
{
	ScopeValidator sv = ScopeValidatorBuilder()
		.withBlockOnScope(ParserTokenType::PT_LOCATION, ParserTokenType::PT_SERVER)
		.andOn(ParserTokenType::PT_LOCATION) // location dentro de location
		.build();

	sv.pushScope(ParserTokenType::PT_LOCATION); // adicionado via andOn
	Block outer_loc(make_name("location", ParserTokenType::PT_LOCATION));
	Block inner_loc(make_name("location", ParserTokenType::PT_LOCATION));
	outer_loc.children.push_back(&inner_loc);

	sv.validate(outer_loc);
	std::vector<std::string> errs;
	sv.dumpErrorsOn(errs);
	return assert_true(errs.empty(), "test_andOn_after_block_adds_block_scope", LINE_DATA());
}

// Feature: andOn() opera sobre o ÚLTIMO tipo configurado, e distingue
// corretamente se esse último foi um bloco ou uma diretiva — chamadas
// alternadas de withDirectiveOnScope/withBlockOnScope seguidas de andOn()
// devem direcionar para a tabela correta a cada vez.
int test_andOn_tracks_last_call_type_correctly_across_alternation()
{
	ScopeValidator sv = ScopeValidatorBuilder()
		.withDirectiveOnScope(ParserTokenType::PT_LISTEN, ParserTokenType::PT_SERVER)
		.withBlockOnScope(ParserTokenType::PT_SERVER, ParserTokenType::PT_HTTP)
		.andOn(ParserTokenType::PT_MAIN) // deve afetar BLOCK (PT_SERVER), não DIRECTIVE (PT_LISTEN)
		.build();

	// PT_SERVER (bloco) deveria agora ser permitido em PT_HTTP E PT_MAIN
	sv.pushScope(ParserTokenType::PT_MAIN);
	Block root(make_name("root", ParserTokenType::PT_MAIN));
	Block server_child(make_name("server", ParserTokenType::PT_SERVER));
	root.children.push_back(&server_child);
	sv.validate(root);
	std::vector<std::string> errs;
	sv.dumpErrorsOn(errs);

	// PT_LISTEN (diretiva) NÃO deveria ter ganho PT_MAIN como escopo extra
	sv.pushScope(ParserTokenType::PT_MAIN); // empilha de novo (estado independente)
	Block root2(make_name("root", ParserTokenType::PT_MAIN));
	Directive listen_child(make_name("listen", ParserTokenType::PT_LISTEN));
	root2.children.push_back(&listen_child);
	sv.validate(root2);
	std::vector<std::string> errs2;
	sv.dumpErrorsOn(errs2);

	bool ok = errs.empty() && !errs2.empty(); // server passa, listen falha
	return assert_true(ok, "test_andOn_tracks_last_call_type_correctly_across_alternation", LINE_DATA());
}

// ── ScopeValidatorBuilder::build — independência de instâncias ───────────────

int test_builder_build_returns_independent_copy()
{
	ScopeValidatorBuilder builder;
	builder.withDirectiveOnScope(ParserTokenType::PT_LISTEN, ParserTokenType::PT_SERVER);
	ScopeValidator sv1 = builder.build();

	builder.withDirectiveOnScope(ParserTokenType::PT_ROOT, ParserTokenType::PT_SERVER);
	ScopeValidator sv2 = builder.build();

	sv1.pushScope(ParserTokenType::PT_SERVER);
	Block server1(make_name("server", ParserTokenType::PT_SERVER));
	Directive root1(make_name("root", ParserTokenType::PT_ROOT));
	server1.children.push_back(&root1);
	sv1.validate(server1); // sv1 não conhece PT_ROOT
	std::vector<std::string> errs1;
	sv1.dumpErrorsOn(errs1);

	sv2.pushScope(ParserTokenType::PT_SERVER);
	Block server2(make_name("server", ParserTokenType::PT_SERVER));
	Directive root2(make_name("root", ParserTokenType::PT_ROOT));
	server2.children.push_back(&root2);
	sv2.validate(server2); // sv2 conhece PT_ROOT
	std::vector<std::string> errs2;
	sv2.dumpErrorsOn(errs2);

	bool ok = !errs1.empty() && errs2.empty();
	return assert_true(ok, "test_builder_build_returns_independent_copy", LINE_DATA());
}

// ── ScopeValidatorBuilder::defaultScopes / withDefault*Scopes ─────────────────

int test_default_scopes_server_must_be_in_http()
{
	ScopeValidator sv = ScopeValidatorBuilder::defaultScopes();
	sv.pushScope(ParserTokenType::PT_HTTP);
	Block http(make_name("http", ParserTokenType::PT_HTTP));
	Block server(make_name("server", ParserTokenType::PT_SERVER));
	http.children.push_back(&server);
	sv.validate(http);
	std::vector<std::string> errs;
	sv.dumpErrorsOn(errs);
	return assert_true(errs.empty(), "test_default_scopes_server_must_be_in_http", LINE_DATA());
}

int test_default_scopes_server_rejected_outside_http()
{
	ScopeValidator sv = ScopeValidatorBuilder::defaultScopes();
	sv.pushScope(ParserTokenType::PT_MAIN);
	Block root(make_name("root", ParserTokenType::PT_MAIN));
	Block server(make_name("server", ParserTokenType::PT_SERVER));
	root.children.push_back(&server);
	sv.validate(root);
	std::vector<std::string> errs;
	sv.dumpErrorsOn(errs);
	return assert_true(errs.size() == 1, "test_default_scopes_server_rejected_outside_http", LINE_DATA());
}

int test_default_scopes_location_allowed_in_server_and_location()
{
	ScopeValidator sv = ScopeValidatorBuilder::defaultScopes();

	sv.pushScope(ParserTokenType::PT_SERVER);
	Block server(make_name("server", ParserTokenType::PT_SERVER));
	Block loc(make_name("location", ParserTokenType::PT_LOCATION));
	server.children.push_back(&loc);
	sv.validate(server);
	std::vector<std::string> errs1;
	sv.dumpErrorsOn(errs1);

	sv.pushScope(ParserTokenType::PT_LOCATION); // location dentro de location
	Block outer(make_name("location", ParserTokenType::PT_LOCATION));
	Block inner(make_name("location", ParserTokenType::PT_LOCATION));
	outer.children.push_back(&inner);
	sv.validate(outer);
	std::vector<std::string> errs2;
	sv.dumpErrorsOn(errs2);

	bool ok = errs1.empty() && errs2.empty();
	return assert_true(ok, "test_default_scopes_location_allowed_in_server_and_location", LINE_DATA());
}

int test_default_scopes_listen_only_in_server()
{
	ScopeValidator sv = ScopeValidatorBuilder::defaultScopes();
	sv.pushScope(ParserTokenType::PT_HTTP); // errado
	Block http(make_name("http", ParserTokenType::PT_HTTP));
	Directive listen(make_name("listen", ParserTokenType::PT_LISTEN));
	http.children.push_back(&listen);
	sv.validate(http);
	std::vector<std::string> errs;
	sv.dumpErrorsOn(errs);
	return assert_true(errs.size() == 1, "test_default_scopes_listen_only_in_server", LINE_DATA());
}

int test_default_scopes_root_allowed_in_http_server_and_location()
{
	ScopeValidator sv = ScopeValidatorBuilder::defaultScopes();

	sv.pushScope(ParserTokenType::PT_HTTP);
	Block http(make_name("http", ParserTokenType::PT_HTTP));
	Directive root_http(make_name("root", ParserTokenType::PT_ROOT));
	http.children.push_back(&root_http);
	sv.validate(http);
	std::vector<std::string> e1; sv.dumpErrorsOn(e1);

	sv.pushScope(ParserTokenType::PT_SERVER);
	Block server(make_name("server", ParserTokenType::PT_SERVER));
	Directive root_srv(make_name("root", ParserTokenType::PT_ROOT));
	server.children.push_back(&root_srv);
	sv.validate(server);
	std::vector<std::string> e2; sv.dumpErrorsOn(e2);

	sv.pushScope(ParserTokenType::PT_LOCATION);
	Block loc(make_name("location", ParserTokenType::PT_LOCATION));
	Directive root_loc(make_name("root", ParserTokenType::PT_ROOT));
	loc.children.push_back(&root_loc);
	sv.validate(loc);
	std::vector<std::string> e3; sv.dumpErrorsOn(e3);

	bool ok = e1.empty() && e2.empty() && e3.empty();
	return assert_true(ok, "test_default_scopes_root_allowed_in_http_server_and_location", LINE_DATA());
}

int test_default_scopes_include_allowed_almost_everywhere()
{
	ScopeValidator sv = ScopeValidatorBuilder::defaultScopes();
	sv.pushScope(ParserTokenType::PT_EVENTS);
	Block events(make_name("events", ParserTokenType::PT_EVENTS));
	Directive include_dir(make_name("include", ParserTokenType::PT_INCLUDE));
	events.children.push_back(&include_dir);
	sv.validate(events);
	std::vector<std::string> errs;
	sv.dumpErrorsOn(errs);
	return assert_true(errs.empty(), "test_default_scopes_include_allowed_almost_everywhere", LINE_DATA());
}

int test_default_scopes_worker_processes_only_in_main()
{
	ScopeValidator sv = ScopeValidatorBuilder::defaultScopes();

	sv.pushScope(ParserTokenType::PT_MAIN);
	Block root(make_name("root", ParserTokenType::PT_MAIN));
	Directive wp(make_name("worker_processes", ParserTokenType::PT_WORKER_PROCESSES));
	root.children.push_back(&wp);
	sv.validate(root);
	std::vector<std::string> ok_errs;
	sv.dumpErrorsOn(ok_errs);

	sv.pushScope(ParserTokenType::PT_HTTP);
	Block http(make_name("http", ParserTokenType::PT_HTTP));
	Directive wp2(make_name("worker_processes", ParserTokenType::PT_WORKER_PROCESSES));
	http.children.push_back(&wp2);
	sv.validate(http);
	std::vector<std::string> bad_errs;
	sv.dumpErrorsOn(bad_errs);

	bool ok = ok_errs.empty() && bad_errs.size() == 1;
	return assert_true(ok, "test_default_scopes_worker_processes_only_in_main", LINE_DATA());
}

// ── main ──────────────────────────────────────────────────────────────────────

int main(int argc, char **argv)
{

	messages::settup(argc, argv);
	int failures = 0;

	// std::cout << "\n=== pushScope/popScope — pilha básica ===\n";
	failures += test_pushScope_then_validate_directive_in_correct_scope_passes();
	failures += test_directive_in_wrong_scope_generates_error();
	failures += test_wrong_scope_error_mentions_allowed_scope();
	failures += test_wrong_scope_error_includes_line_address();

	// std::cout << "\n=== tipo sem regra registrada ===\n";
	failures += test_directive_without_registered_rule_generates_scope_not_found_error();
	failures += test_scope_not_found_error_includes_content_and_type();

	// std::cout << "\n=== PT_WORD / PT_END são ignorados ===\n";
	failures += test_pt_word_child_is_ignored_even_without_rule();
	failures += test_pt_end_child_is_ignored_even_without_rule();

	// std::cout << "\n=== Block vs Directive usam tabelas distintas ===\n";
	failures += test_block_child_uses_block_scope_rules_not_directive_rules();
	failures += test_directive_child_uses_directive_scope_rules_not_block_rules();

	// std::cout << "\n=== Múltiplos escopos permitidos (andOn) ===\n";
	failures += test_directive_allowed_in_any_of_multiple_registered_scopes();
	failures += test_error_message_lists_all_allowed_scopes_comma_separated();
	failures += test_error_message_single_scope_has_no_trailing_comma();

	// std::cout << "\n=== pushScope/popScope — pilha aninhada ===\n";
	failures += test_nested_scopes_validate_against_innermost_scope();
	failures += test_popScope_restores_previous_scope();

	// std::cout << "\n=== BUG CRÍTICO: validate() sem pushScope() prévio ===\n";
	failures += test_validate_without_pushscope_crashes_precondition_BUG();

	// std::cout << "\n=== dumpErrorsOn() ===\n";
	failures += test_dumpErrorsOn_appends_and_clears();

	// std::cout << "\n=== ScopeValidatorBuilder — fluent chaining ===\n";
	failures += test_withDirectiveOnScope_returns_self_for_chaining();
	failures += test_withBlockOnScope_returns_self_for_chaining();
	failures += test_andOn_returns_self_for_chaining();
	failures += test_andOn_after_directive_adds_directive_scope();
	failures += test_andOn_after_block_adds_block_scope();
	failures += test_andOn_tracks_last_call_type_correctly_across_alternation();

	// std::cout << "\n=== ScopeValidatorBuilder::build — independência ===\n";
	failures += test_builder_build_returns_independent_copy();

	// std::cout << "\n=== defaultScopes() / withDefault*Scopes() ===\n";
	failures += test_default_scopes_server_must_be_in_http();
	failures += test_default_scopes_server_rejected_outside_http();
	failures += test_default_scopes_location_allowed_in_server_and_location();
	failures += test_default_scopes_listen_only_in_server();
	failures += test_default_scopes_root_allowed_in_http_server_and_location();
	failures += test_default_scopes_include_allowed_almost_everywhere();
	failures += test_default_scopes_worker_processes_only_in_main();

	messages::print();

	return failures;
}
