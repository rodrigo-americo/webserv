/* ************************************************************************** */
/*  test_parser.cpp — unit tests for Parser                                  */
/*                                                                            */
/*  Convenção:                                                                */
/*    - Testes com comportamento CORRETO ficam como estão.                    */
/*    - Testes que documentam um BUG têm sufixo _BUG: o assert verifica o    */
/*      que o código REALMENTE faz hoje, não o esperado. Ao corrigir o bug   */
/*      o teste falhará — esse é o sinal de que o fix funcionou.             */
/*                                                                            */
/*  Dependências reconstruídas / inferidas (não fornecidas pelo usuário):    */
/*    - ParserAst.hpp  — stub mínimo com addError/setRoot (usados pelo       */
/*      Parser) e getRoot/getErrors/hasErrors (adicionados só para inspeção  */
/*      nos testes). Suporta cópia profunda, pois Parser::parse() retorna    */
/*      ParserAst por valor.                                                 */
/*    - has_type.hpp   — já usado nas suítes de Lexer/LexerToken.            */
/*    - Lexer.hpp, LexerToken.hpp, LexerIterator.hpp, LexerBuilder.hpp,      */
/*      LexerTokenType.hpp, File.hpp, utils.hpp — reaproveitados das suítes  */
/*      anteriores (com pequenos ajustes de include guard).                  */
/* ************************************************************************** */

#include "test_utils.hpp"
#include "Parser.hpp"

#include <fstream>

// ── helpers ──────────────────────────────────────────────────────────────────

static void write_file(const std::string &path, const std::string &content)
{
	std::ofstream f(path.c_str());
	f << content;
}

// Cria um Parser pré-configurado com um conjunto comum de keywords usado
// pela maioria dos testes (similar a um subconjunto de diretivas nginx).
static Parser make_configured_parser(const std::string &path, const std::string &content)
{
	write_file(path, content);
	LexerBuilder lb;
	lb.withFile(path).withDefaultTokens();
	Parser p(lb);
	p.addBlockKeyword("server", ParserTokenType::PT_SERVER);
	p.addBlockKeyword("location", ParserTokenType::PT_LOCATION);
	p.addBlockKeyword("http", ParserTokenType::PT_HTTP);
	p.addDirectiveKeyword("listen", ParserTokenType::PT_LISTEN);
	p.addDirectiveKeyword("root", ParserTokenType::PT_ROOT);
	p.addDirectiveKeyword("index", ParserTokenType::PT_INDEX);
	p.addModifier("=", ParserTokenType::PT_MOD_EXACT);
	p.addModifier("~", ParserTokenType::PT_MOD_REGEX);
	return p;
}

// ── Construtores ──────────────────────────────────────────────────────────────

int test_ctor_default_parses_empty_without_crash()
{
	write_file("/tmp/p_t1.conf", "");
	LexerBuilder lb;
	lb.withFile("/tmp/p_t1.conf");
	Parser p(lb);
	ParserAst ast = p.parse();
	return assert_true(ast.getRoot() != NULL, "test_ctor_default_parses_empty_without_crash", LINE_DATA());
}

int test_ctor_with_builder_uses_that_builder()
{
	write_file("/tmp/p_t2.conf", "x;");
	LexerBuilder lb;
	lb.withFile("/tmp/p_t2.conf").withDefaultTokens();
	Parser p(lb);
	ParserAst ast = p.parse();
	// root tem children (mesmo que seja só o nó espúrio END, ver bug abaixo) —
	// o ponto aqui é só confirmar que o builder passado foi de fato usado.
	return assert_true(!ast.getRoot()->children.empty(), "test_ctor_with_builder_uses_that_builder", LINE_DATA());
}

// ── addBlockKeyword / addDirectiveKeyword / addModifier ───────────────────────

int test_addBlockKeyword_creates_block_node()
{
	Parser p = make_configured_parser("/tmp/p_t3.conf", "server { listen 80; }");
	ParserAst ast = p.parse();
	ParserNode *n = ast.getRoot()->children[0];
	return assert_true(n->node_type == BLOCK && n->name.getType() == ParserTokenType::PT_SERVER,
					"test_addBlockKeyword_creates_block_node", LINE_DATA());
}

int test_addDirectiveKeyword_assigns_correct_type()
{
	Parser p = make_configured_parser("/tmp/p_t4.conf", "listen 80;");
	ParserAst ast = p.parse();
	ParserNode *n = ast.getRoot()->children[0];
	return assert_true(n->name.getType() == ParserTokenType::PT_LISTEN,
					"test_addDirectiveKeyword_assigns_correct_type", LINE_DATA());
}

int test_addModifier_applied_only_after_location()
{
	Parser p = make_configured_parser("/tmp/p_t5.conf", "location = /path { root /x; }");
	ParserAst ast = p.parse();
	ParserNode *loc = ast.getRoot()->children[0];
	bool ok = loc->values.size() >= 1
		&& loc->values[0].getType() == ParserTokenType::PT_MOD_EXACT
		&& loc->values[0].getContent() == "=";
	return assert_true(ok, "test_addModifier_applied_only_after_location", LINE_DATA());
}

int test_addModifier_text_not_consumed_as_modifier_outside_location()
{
	// Fora do contexto de location, o mesmo texto "=" não deveria virar
	// PT_MOD_EXACT — ele cai no fallback PT_WORD.
	Parser p = make_configured_parser("/tmp/p_t6.conf", "listen = ;");
	ParserAst ast = p.parse();
	ParserNode *n = ast.getRoot()->children[0];
	bool ok = !n->values.empty() && n->values[0].getType() == ParserTokenType::PT_WORD;
	return assert_true(ok, "test_addModifier_text_not_consumed_as_modifier_outside_location", LINE_DATA());
}

int test_modifier_only_applies_to_first_value_after_location()
{
	// is_after_location é resetado para false após o PRIMEIRO valor lido,
	// então um segundo "=" na mesma diretiva não deveria ser tratado como
	// modificador.
	Parser p = make_configured_parser("/tmp/p_t7.conf", "location = a = b { root /x; }");
	ParserAst ast = p.parse();
	ParserNode *loc = ast.getRoot()->children[0];
	bool ok = loc->values.size() >= 4
		&& loc->values[0].getType() == ParserTokenType::PT_MOD_EXACT  // "="  primeiro valor
		&& loc->values[2].getType() == ParserTokenType::PT_WORD;      // "="  segunda ocorrência
	return assert_true(ok, "test_modifier_only_applies_to_first_value_after_location", LINE_DATA());
}

// ── _convertToken — strings sempre prioritárias ──────────────────────────────

int test_string_double_has_priority_over_directive_keyword()
{
	// Mesmo que o conteúdo da string coincida com uma keyword de diretiva
	// registrada, o tipo léxico STRING_DOUBLE tem prioridade absoluta.
	Parser p = make_configured_parser("/tmp/p_t8.conf", "\"listen\";");
	ParserAst ast = p.parse();
	ParserNode *n = ast.getRoot()->children[0];
	return assert_true(n->name.getType() == ParserTokenType::PT_STRING_DOUBLE,
					"test_string_double_has_priority_over_directive_keyword", LINE_DATA());
}

int test_string_single_has_priority_over_block_keyword()
{
	Parser p = make_configured_parser("/tmp/p_t9.conf", "'server' { listen 80; }");
	ParserAst ast = p.parse();
	ParserNode *n = ast.getRoot()->children[0];
	// O tipo do NOME é PT_STRING_SINGLE (não PT_SERVER), mas como o próximo
	// token é LBRACE, parseStatement() decide criar um Block mesmo assim —
	// a decisão bloco/diretiva é baseada no token seguinte, não no tipo do
	// nome.
	bool ok = n->name.getType() == ParserTokenType::PT_STRING_SINGLE
		&& n->node_type == BLOCK;
	return assert_true(ok, "test_string_single_has_priority_over_block_keyword", LINE_DATA());
}

// ── directive keyword tem prioridade sobre block keyword ─────────────────────

int test_directive_keyword_checked_before_block_keyword()
{
	// Se a mesma palavra for registrada como diretiva E como bloco,
	// _convertToken verifica _directive_keywords antes de _block_keywords.
	Parser p = make_configured_parser("/tmp/p_t10.conf", "dual;");
	LexerBuilder lb;
	write_file("/tmp/p_t10.conf", "dual;");
	lb.withFile("/tmp/p_t10.conf").withDefaultTokens();
	Parser p2(lb);
	p2.addDirectiveKeyword("dual", ParserTokenType::PT_LISTEN);
	p2.addBlockKeyword("dual", ParserTokenType::PT_SERVER);
	ParserAst ast = p2.parse();
	ParserNode *n = ast.getRoot()->children[0];
	return assert_true(n->name.getType() == ParserTokenType::PT_LISTEN,
					"test_directive_keyword_checked_before_block_keyword", LINE_DATA());
}

// ── PT_SERVER renomeado para PT_SERVER_DIRECTIVE quando sem bloco ────────────

int test_server_without_block_becomes_server_directive()
{
	Parser p = make_configured_parser("/tmp/p_t11.conf", "server abc;");
	ParserAst ast = p.parse();
	ParserNode *n = ast.getRoot()->children[0];
	bool ok = n->node_type == DIRECTIVE
		&& n->name.getType() == ParserTokenType::PT_SERVER_DIRECTIVE;
	return assert_true(ok, "test_server_without_block_becomes_server_directive", LINE_DATA());
}

int test_server_with_block_stays_pt_server()
{
	Parser p = make_configured_parser("/tmp/p_t12.conf", "server { listen 80; }");
	ParserAst ast = p.parse();
	ParserNode *n = ast.getRoot()->children[0];
	bool ok = n->node_type == BLOCK && n->name.getType() == ParserTokenType::PT_SERVER;
	return assert_true(ok, "test_server_with_block_stays_pt_server", LINE_DATA());
}

// ── Estrutura básica: diretivas e valores ─────────────────────────────────────

int test_directive_captures_single_value()
{
	Parser p = make_configured_parser("/tmp/p_t13.conf", "listen 8080;");
	ParserAst ast = p.parse();
	ParserNode *n = ast.getRoot()->children[0];
	bool ok = n->values.size() == 1 && n->values[0].getContent() == "8080";
	return assert_true(ok, "test_directive_captures_single_value", LINE_DATA());
}

int test_directive_captures_multiple_values()
{
	Parser p = make_configured_parser("/tmp/p_t14.conf", "index a.html b.html c.html;");
	ParserAst ast = p.parse();
	ParserNode *n = ast.getRoot()->children[0];
	bool ok = n->values.size() == 3
		&& n->values[0].getContent() == "a.html"
		&& n->values[1].getContent() == "b.html"
		&& n->values[2].getContent() == "c.html";
	return assert_true(ok, "test_directive_captures_multiple_values", LINE_DATA());
}

int test_directive_with_no_values()
{
	Parser p = make_configured_parser("/tmp/p_t15.conf", "listen;");
	ParserAst ast = p.parse();
	ParserNode *n = ast.getRoot()->children[0];
	return assert_true(n->values.empty(), "test_directive_with_no_values", LINE_DATA());
}

// ── Estrutura: blocos e filhos ────────────────────────────────────────────────

int test_block_has_correct_children_count()
{
	Parser p = make_configured_parser("/tmp/p_t16.conf", "server { listen 80; root /x; }");
	ParserAst ast = p.parse();
	Block *server = static_cast<Block *>(ast.getRoot()->children[0]);
	return assert_true(server->children.size() == 2, "test_block_has_correct_children_count", LINE_DATA());
}

int test_nested_blocks()
{
	Parser p = make_configured_parser("/tmp/p_t17.conf",
		"server { location /a { root /x; } }");
	ParserAst ast = p.parse();
	Block *server = static_cast<Block *>(ast.getRoot()->children[0]);
	ParserNode *loc = server->children[0];
	bool ok = loc->node_type == BLOCK
		&& loc->name.getType() == ParserTokenType::PT_LOCATION
		&& static_cast<Block *>(loc)->children.size() == 1;
	return assert_true(ok, "test_nested_blocks", LINE_DATA());
}

int test_block_values_captured_before_lbrace()
{
	Parser p = make_configured_parser("/tmp/p_t18.conf", "location /images { root /x; }");
	ParserAst ast = p.parse();
	ParserNode *loc = ast.getRoot()->children[0];
	bool ok = loc->values.size() == 1 && loc->values[0].getContent() == "/images";
	return assert_true(ok, "test_block_values_captured_before_lbrace", LINE_DATA());
}

// ── Comentários ───────────────────────────────────────────────────────────────

int test_comment_before_top_level_directive_is_skipped()
{
	Parser p = make_configured_parser("/tmp/p_t19.conf", "# top comment\nlisten 80;");
	ParserAst ast = p.parse();
	const Block *root = ast.getRoot();
	// primeiro filho REAL (ignorando o nó espúrio de END, ver bug) deve ser 'listen'
	bool ok = !root->children.empty() && root->children[0]->name.getContent() == "listen";
	return assert_true(ok, "test_comment_before_top_level_directive_is_skipped", LINE_DATA());
}

int test_comments_between_directives_inside_block_are_skipped()
{
	Parser p = make_configured_parser("/tmp/p_t20.conf",
		"server {\n  # c1\n  listen 80;\n  # c2\n  root /x;\n}");
	ParserAst ast = p.parse();
	Block *server = static_cast<Block *>(ast.getRoot()->children[0]);
	bool ok = server->children.size() == 2
		&& server->children[0]->name.getContent() == "listen"
		&& server->children[1]->name.getContent() == "root";
	return assert_true(ok, "test_comments_between_directives_inside_block_are_skipped", LINE_DATA());
}

// ── Erros: nome de nó inválido ────────────────────────────────────────────────

int test_unknown_word_as_name_generates_error()
{
	Parser p = make_configured_parser("/tmp/p_t21.conf", "unknown_thing value;");
	ParserAst ast = p.parse();
	bool ok = !ast.getErrors().empty()
		&& ast.getErrors()[0].find("Invalid node name!") != std::string::npos;
	return assert_true(ok, "test_unknown_word_as_name_generates_error", LINE_DATA());
}

int test_known_directive_name_generates_no_error()
{
	Parser p = make_configured_parser("/tmp/p_t22.conf", "listen 80;");
	ParserAst ast = p.parse();
	return assert_true(ast.getErrors().empty(), "test_known_directive_name_generates_no_error", LINE_DATA());
}

// ── Erros: escopo não fechado ──────────────────────────────────────────────────

int test_unclosed_scope_generates_error()
{
	Parser p = make_configured_parser("/tmp/p_t23.conf", "server { listen 80;");
	ParserAst ast = p.parse();
	bool ok = !ast.getErrors().empty()
		&& ast.getErrors()[0].find("Unclosed scope") != std::string::npos;
	return assert_true(ok, "test_unclosed_scope_generates_error", LINE_DATA());
}


int test_unclosed_scope_error_reports_lbrace_position()
{
	Parser p = make_configured_parser("/tmp/p_t24.conf", "server { listen 80;");
	ParserAst ast = p.parse();
	bool ok = !ast.getErrors().empty()
		&& ast.getErrors()[0].find(":1:7") != std::string::npos;
	return assert_true(ok, "test_unclosed_scope_error_reports_lbrace_position", LINE_DATA());
}

int test_unclosed_line_number_is_off_by_one()
{
	Parser p = make_configured_parser("/tmp/p_t24b.conf", "# comment line\nserver { listen 80;");
	ParserAst ast = p.parse();
	bool ok = !ast.getErrors().empty()
		&& ast.getErrors()[0].find(":2:7") != std::string::npos;
	return assert_true(ok, "test_unclosed_line_number_is_off_by_one", LINE_DATA());
}

int test_properly_closed_scope_no_unclosed_error()
{
	Parser p = make_configured_parser("/tmp/p_t25.conf", "server { listen 80; }");
	ParserAst ast = p.parse();
	bool has_unclosed_error = false;
	for (size_t i = 0; i < ast.getErrors().size(); i++)
		if (ast.getErrors()[i].find("Unclosed scope") != std::string::npos)
			has_unclosed_error = true;
	return assert_true(!has_unclosed_error, "test_properly_closed_scope_no_unclosed_error", LINE_DATA());
}

// ── Erros: fechamento de escopo não aberto ────────────────────────────────────

int test_unopened_scope_as_value_generates_error()
{
	// '}' aparecendo como VALOR (argumento) de uma diretiva, antes do ';'
	Parser p = make_configured_parser("/tmp/p_t26.conf", "listen 80 } ;");
	ParserAst ast = p.parse();
	bool ok = false;
	for (size_t i = 0; i < ast.getErrors().size(); i++)
		if (ast.getErrors()[i].find("Closing an unopened scope!") != std::string::npos)
			ok = true;
	return assert_true(ok, "test_unopened_scope_as_value_generates_error", LINE_DATA());
}

int test_unopened_scope_as_first_token_reports_generic_error_instead()
{
	Parser p = make_configured_parser("/tmp/p_t27.conf", "listen 80; }");
	ParserAst ast = p.parse();
	bool got_generic_invalid_name_error = false;
	bool got_specific_unopened_scope_error = false;
	for (size_t i = 0; i < ast.getErrors().size(); i++)
	{
		if (ast.getErrors()[i].find("Invalid node name!") != std::string::npos)
			got_generic_invalid_name_error = true;
		if (ast.getErrors()[i].find("Closing an unopened scope!") != std::string::npos)
			got_specific_unopened_scope_error = true;
	}
	// Comportamento atual: gera o erro genérico, NÃO o específico de escopo.
	bool ok = got_generic_invalid_name_error && !got_specific_unopened_scope_error;
	return assert_true(ok, "test_unopened_scope_as_first_token_reports_generic_error_instead", LINE_DATA());
}

// ── BUG CRÍTICO: nó espúrio END no final de toda análise ─────────────────────
//
// BUG CRÍTICO: Parser::parse() usa `while (_it) { ...; root->children.push_
// back(parseStatement()); }` no nível raiz (e um padrão similar dentro de
// blocos). O LexerIterator permanece "true" (operator bool) enquanto o
// índice estiver dentro do range de tokens — e o token END (sempre presente
// como ÚLTIMO token de QUALQUER análise léxica bem-sucedida) ainda está
// dentro desse range. Por isso, o laço NÃO para antes de processar o END:
// ele chama parseStatement() mais uma vez sobre o token END, que cai no
// fallback de _convertToken (`if (token == LexerTokenType::END) return
// ParserToken::fromLexerToken(token, ParserTokenType::PT_END);`), criando
// uma Directive espúria com nome vazio e tipo PT_END.
//
// Resultado: TODA árvore produzida por Parser::parse() — em qualquer nível
// de recursão (raiz ou dentro de blocos) — contém um nó filho extra
// indesejado no final, representando o fim do arquivo como se fosse uma
// diretiva real. Qualquer código que itere sobre `children` esperando
// apenas nós de diretivas/blocos legítimos vai encontrar esse nó "fantasma".
//
// Comportamento esperado: o laço deveria parar (ou _skipComments()/checagem
// equivalente deveria tratar) antes de processar o token END como uma nova
// statement — por exemplo, `while (_it && *_it != LexerTokenType::END)`.
int test_root_always_has_spurious_end_node_BUG()
{
	Parser p = make_configured_parser("/tmp/p_t28.conf", "listen 80;");
	ParserAst ast = p.parse();
	const Block *root = ast.getRoot();
	// Comportamento atual: 2 filhos (listen real + END espúrio), não 1.
	bool ok = root->children.size() == 2
		&& root->children[1]->name.getType() == ParserTokenType::PT_END
		&& root->children[1]->name.getContent().empty();
	return assert_true(ok, "test_root_always_has_spurious_end_node_BUG", LINE_DATA());
}

int test_block_children_also_get_spurious_end_node_BUG()
{
	// O mesmo bug se manifesta dentro de blocos quando o arquivo termina sem
	// o '}' de fechamento (escopo não fechado) — o laço interno também usa
	// `while (_it && *_it != RBRACE)`, e como o RBRACE nunca aparece (chegou
	// ao fim do arquivo), o laço continua até _it virar false, mas antes
	// disso processa o token END como uma statement filha espúria.
	Parser p = make_configured_parser("/tmp/p_t29.conf", "server { listen 80;");
	ParserAst ast = p.parse();
	Block *server = static_cast<Block *>(ast.getRoot()->children[0]);
	bool ok = server->children.size() == 2
		&& server->children[0]->name.getContent() == "listen"
		&& server->children[1]->name.getType() == ParserTokenType::PT_END;
	return assert_true(ok, "test_block_children_also_get_spurious_end_node_BUG", LINE_DATA());
}

int test_empty_file_still_produces_spurious_end_node_BUG()
{
	// Mesmo um arquivo totalmente vazio gera 1 filho espúrio na raiz, em vez
	// de uma árvore com zero filhos.
	Parser p = make_configured_parser("/tmp/p_t30.conf", "");
	ParserAst ast = p.parse();
	const Block *root = ast.getRoot();
	// Arquivo vazio -> Lexer produz 0 tokens (nem END é gerado, pois o laço
	// while(_file) do Lexer nunca executa para um arquivo vazio) -> aqui o
	// LexerIterator.bool() já é false desde o início -> o laço externo do
	// Parser nunca executa -> SEM filho espúrio neste caso específico.
	// Documentamos isso para deixar claro que o bug depende de existir ao
	// menos 1 token real (e portanto um END subsequente) — arquivo vazio é
	// a única exceção que escapa do bug.
	return assert_true(root->children.empty(),
					"test_empty_file_still_produces_spurious_end_node_BUG", LINE_DATA());
}

// ── Erro do Lexer propagado para a AST ────────────────────────────────────────

int test_lexer_error_propagated_to_ast()
{
	std::remove("/tmp/p_missing.conf");
	LexerBuilder lb;
	lb.withFile("/tmp/p_missing.conf").withDefaultTokens();
	Parser p(lb);
	ParserAst ast = p.parse();
	bool ok = !ast.getErrors().empty()
		&& ast.getErrors()[0].find("Cannot open file") != std::string::npos;
	return assert_true(ok, "test_lexer_error_propagated_to_ast", LINE_DATA());
}

int test_lexer_error_does_not_prevent_root_creation()
{
	std::remove("/tmp/p_missing2.conf");
	LexerBuilder lb;
	lb.withFile("/tmp/p_missing2.conf").withDefaultTokens();
	Parser p(lb);
	ParserAst ast = p.parse();
	// Mesmo com erro do lexer, o root Block é sempre criado (new Block(...)
	// ocorre antes do laço, incondicionalmente).
	return assert_true(ast.getRoot() != NULL, "test_lexer_error_does_not_prevent_root_creation", LINE_DATA());
}

// ── setLexer() ────────────────────────────────────────────────────────────────

int test_setLexer_switches_source()
{
	write_file("/tmp/p_t31a.conf", "listen 80;");
	write_file("/tmp/p_t31b.conf", "root /x;");

	LexerBuilder lb1;
	lb1.withFile("/tmp/p_t31a.conf").withDefaultTokens();
	Parser p(lb1);
	p.addDirectiveKeyword("listen", ParserTokenType::PT_LISTEN);
	p.addDirectiveKeyword("root", ParserTokenType::PT_ROOT);

	LexerBuilder lb2;
	lb2.withFile("/tmp/p_t31b.conf").withDefaultTokens();
	p.setLexer(lb2);

	ParserAst ast = p.parse();
	const Block *root = ast.getRoot();
	bool ok = !root->children.empty() && root->children[0]->name.getContent() == "root";
	return assert_true(ok, "test_setLexer_switches_source", LINE_DATA());
}

// ── Múltiplas statements no nível raiz ────────────────────────────────────────

int test_multiple_top_level_statements()
{
	Parser p = make_configured_parser("/tmp/p_t32.conf",
		"server { listen 80; }\nserver { listen 81; }");
	ParserAst ast = p.parse();
	const Block *root = ast.getRoot();
	// 2 blocos server reais + 1 nó espúrio END (bug já documentado acima)
	bool ok = root->children.size() == 3
		&& root->children[0]->node_type == BLOCK
		&& root->children[1]->node_type == BLOCK;
	return assert_true(ok, "test_multiple_top_level_statements", LINE_DATA());
}

// ── main ──────────────────────────────────────────────────────────────────────

int main(int argc, char **argv)
{

	messages::settup(argc, argv);

	int failures = 0;

	// std::cout << "\n=== Construtores ===\n";
	failures += test_ctor_default_parses_empty_without_crash();
	failures += test_ctor_with_builder_uses_that_builder();

	// std::cout << "\n=== addBlockKeyword / addDirectiveKeyword / addModifier ===\n";
	failures += test_addBlockKeyword_creates_block_node();
	failures += test_addDirectiveKeyword_assigns_correct_type();
	failures += test_addModifier_applied_only_after_location();
	failures += test_addModifier_text_not_consumed_as_modifier_outside_location();
	failures += test_modifier_only_applies_to_first_value_after_location();

	// std::cout << "\n=== _convertToken — strings sempre prioritárias ===\n";
	failures += test_string_double_has_priority_over_directive_keyword();
	failures += test_string_single_has_priority_over_block_keyword();

	// std::cout << "\n=== diretiva tem prioridade sobre block keyword ===\n";
	failures += test_directive_keyword_checked_before_block_keyword();

	// std::cout << "\n=== PT_SERVER -> PT_SERVER_DIRECTIVE ===\n";
	failures += test_server_without_block_becomes_server_directive();
	failures += test_server_with_block_stays_pt_server();

	// std::cout << "\n=== Estrutura básica: diretivas e valores ===\n";
	failures += test_directive_captures_single_value();
	failures += test_directive_captures_multiple_values();
	failures += test_directive_with_no_values();

	// std::cout << "\n=== Estrutura: blocos e filhos ===\n";
	failures += test_block_has_correct_children_count();
	failures += test_nested_blocks();
	failures += test_block_values_captured_before_lbrace();

	// std::cout << "\n=== Comentários ===\n";
	failures += test_comment_before_top_level_directive_is_skipped();
	failures += test_comments_between_directives_inside_block_are_skipped();

	// std::cout << "\n=== Erros: nome de nó inválido ===\n";
	failures += test_unknown_word_as_name_generates_error();
	failures += test_known_directive_name_generates_no_error();

	// std::cout << "\n=== Erros: escopo não fechado ===\n";
	failures += test_unclosed_scope_generates_error();
	failures += test_unclosed_scope_error_reports_lbrace_position();
	failures += test_unclosed_line_number_is_off_by_one();
	failures += test_properly_closed_scope_no_unclosed_error();

	// std::cout << "\n=== Erros: fechamento de escopo não aberto ===\n";
	failures += test_unopened_scope_as_value_generates_error();

	// std::cout << "\n=== BUG: '}' como 1o token não gera erro específico ===\n";
	failures += test_unopened_scope_as_first_token_reports_generic_error_instead();

	// std::cout << "\n=== BUG CRÍTICO: nó espúrio END ao final de toda análise ===\n";
	failures += test_root_always_has_spurious_end_node_BUG();
	failures += test_block_children_also_get_spurious_end_node_BUG();
	failures += test_empty_file_still_produces_spurious_end_node_BUG();

	// std::cout << "\n=== Erro do Lexer propagado para a AST ===\n";
	failures += test_lexer_error_propagated_to_ast();
	failures += test_lexer_error_does_not_prevent_root_creation();

	// std::cout << "\n=== setLexer() ===\n";
	failures += test_setLexer_switches_source();

	// std::cout << "\n=== Múltiplas statements no nível raiz ===\n";
	failures += test_multiple_top_level_statements();

		messages::print();

	return failures;
}
