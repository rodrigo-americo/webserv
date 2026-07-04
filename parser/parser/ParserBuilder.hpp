/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserBuilder.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/31 17:40:15 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/28 19:58:04 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	PARSER_BUILDER_HPP
# define PARSER_BUILDER_HPP

# include "Parser.hpp"

/**
 * @brief Builder responsável pela configuração e criação de parsers.
 *
 * Fornece uma interface fluente para registrar palavras-chave,
 * modificadores e configurações do lexer antes de executar a
 * análise sintática e gerar a AST.
 */
class ParserBuilder
{
private:
	Parser			_parser; // @brief Instância do parser em construção.
	LexerBuilder	_lexer_builder; // @brief Builder utilizado para configurar o lexer.
public:
	// @brief Constrói um builder vazio.
	ParserBuilder(): _parser() {};
	~ParserBuilder() {};

	/**
	 * @brief Cria um builder pré-configurado com as opções padrão.
	 *
	 * Configura:
	 * - Lexer padrão
	 * - Blocos padrão
	 * - Diretivas padrão
	 * - Modificadores padrão
	 *
	 * @return Builder configurado.
	 */
	static	ParserBuilder	defaultBuilder()
	{
		return ParserBuilder()
			.withDefaultBlocks().withDefaultDirectives()
			.withDefaultLexer().withDefaultModifiers();
	}

	/**
	 * @brief Executa o parser e gera a AST.
	 *
	 * @return Árvore sintática produzida pela análise.
	 */
	ParserAst			build() { return _parser.parse(); }

	/**
	 * @brief Configura o lexer padrão.
	 *
	 * Adiciona comentários, delimitadores e aspas utilizados
	 * normalmente em arquivos de configuração compatíveis com Nginx.
	 *
	 * @return Referência para o builder.
	 */
	ParserBuilder	&withDefaultLexer()
	{
		_lexer_builder = LexerBuilder().withDefaultTokens();
		_parser.setLexer(_lexer_builder);
		return *this;
	}

	/**
	 * @brief Registra todas as diretivas padrão suportadas.
	 *
	 * @return Referência para o builder.
	 */
	ParserBuilder	&withDefaultDirectives()
	{
		_parser.addDirectiveKeyword("worker_processes", ParserTokenType::PT_WORKER_PROCESSES);
		_parser.addDirectiveKeyword("error_log", ParserTokenType::PT_ERROR_LOG);
		_parser.addDirectiveKeyword("pid", ParserTokenType::PT_PID);
		_parser.addDirectiveKeyword("use", ParserTokenType::PT_USE);
		_parser.addDirectiveKeyword("multi_accept", ParserTokenType::MULTI_ACCEPT);
		_parser.addDirectiveKeyword("default_type", ParserTokenType::DEFAULT_TYPE);
		_parser.addDirectiveKeyword("sendfile", ParserTokenType::SENDFILE);
		_parser.addDirectiveKeyword("keepalive_timeout", ParserTokenType::KEEPALIVE_TIMEOUT);
		_parser.addDirectiveKeyword("log_format", ParserTokenType::PT_LOG_FORMAT);
		_parser.addDirectiveKeyword("autoindex", ParserTokenType::PT_AUTOINDEX);
		_parser.addDirectiveKeyword("client_max_body_size", ParserTokenType::PT_CLIENT_MAX_BODY_SIZE);
		_parser.addDirectiveKeyword("log_not_found", ParserTokenType::PT_LOG_NOT_FOUND);
		_parser.addDirectiveKeyword("proxy_set_header", ParserTokenType::PT_PROXY_SET_HEADER);
		_parser.addDirectiveKeyword("proxy_cache_bypass", ParserTokenType::PT_PROXY_CACHE_BYPASS);
		_parser.addDirectiveKeyword("fastcgi_index", ParserTokenType::PT_FASTCGI_INDEX);
		_parser.addDirectiveKeyword("fastcgi_param", ParserTokenType::PT_FASTCGI_PARAM);
		_parser.addDirectiveKeyword("cgi_extension", ParserTokenType::PT_CGI_EXTENSION);
		_parser.addDirectiveKeyword("worker_connections", ParserTokenType::PT_WORKER_CONNECTIONS);
		_parser.addDirectiveKeyword("expires", ParserTokenType::PT_EXPIRES);
		_parser.addDirectiveKeyword("access_log", ParserTokenType::PT_ACCESS_LOG);
		_parser.addDirectiveKeyword("error_page", ParserTokenType::PT_ERROR_PAGE);
		_parser.addDirectiveKeyword("try_files", ParserTokenType::PT_TRY_FILES);
		_parser.addDirectiveKeyword("add_header", ParserTokenType::PT_ADD_HEADER);
		_parser.addDirectiveKeyword("listen", ParserTokenType::PT_LISTEN);
		_parser.addDirectiveKeyword("server_name", ParserTokenType::PT_SERVER_NAME);
		_parser.addDirectiveKeyword("root", ParserTokenType::PT_ROOT);
		_parser.addDirectiveKeyword("index", ParserTokenType::PT_INDEX);
		_parser.addDirectiveKeyword("include", ParserTokenType::PT_INCLUDE);
		_parser.addDirectiveKeyword("proxy_pass", ParserTokenType::PT_PROXY_PASS);
		_parser.addDirectiveKeyword("fastcgi_pass", ParserTokenType::PT_FASTCGI_PASS);
		_parser.addDirectiveKeyword("return", ParserTokenType::PT_RETURN);
		_parser.addDirectiveKeyword("rewrite", ParserTokenType::PT_REWRITE);
		_parser.addDirectiveKeyword("ssl_certificate", ParserTokenType::PT_SSL_CERTIFICATE);
		_parser.addDirectiveKeyword("ssl_certificate_key", ParserTokenType::PT_SSL_CERTIFICATE_KEY);
		_parser.addDirectiveKeyword("ssl_protocols", ParserTokenType::PT_SSL_PROTOCOLS);
		_parser.addDirectiveKeyword("ssl_ciphers", ParserTokenType::PT_SSL_CIPHERS);
		_parser.addDirectiveKeyword("upload_dir", ParserTokenType::PT_UPLOAD_DIR);
		_parser.addDirectiveKeyword("allow_methods", ParserTokenType::PT_ALLOW_METHODS);
		return *this;
	}

	/**
	 * @brief Registra todos os blocos padrão suportados.
	 *
	 * Exemplos:
	 *
	 * - http
	 *
	 * - server
	 *
	 * - location
	 *
	 * - upstream
	 *
	 * @return Referência para o builder.
	 */
	ParserBuilder	&withDefaultBlocks()
	{
		_parser.addBlockKeyword("http", ParserTokenType::PT_HTTP);
		_parser.addBlockKeyword("events", ParserTokenType::PT_EVENTS);
		_parser.addBlockKeyword("server", ParserTokenType::PT_SERVER);
		_parser.addBlockKeyword("location", ParserTokenType::PT_LOCATION);
		_parser.addBlockKeyword("upstream", ParserTokenType::PT_UPSTREAM);
		_parser.addBlockKeyword("geo", ParserTokenType::PT_GEO);
		_parser.addBlockKeyword("map", ParserTokenType::PT_MAP);
		_parser.addBlockKeyword("stream", ParserTokenType::PT_STREAM);
		_parser.addBlockKeyword("types", ParserTokenType::PT_TYPES);
		_parser.addBlockKeyword("limit_except", ParserTokenType::PT_LIMIT_EXCEPT);
		return *this;
	}

	/**
	 * @brief Registra os modificadores padrão de location.
	 *
	 * Exemplos:
	 *
	 * - =
	 *
	 * - ~
	 *
	 * - ~*
	 *
	 * - ^~
	 *
	 * @return Referência para o builder.
	 */
	ParserBuilder	&withDefaultModifiers()
	{
		_parser.addModifier("=", ParserTokenType::PT_MOD_EXACT);
		_parser.addModifier("~", ParserTokenType::PT_MOD_REGEX);
		_parser.addModifier("~*", ParserTokenType::PT_MOD_REGEX_CI);
		_parser.addModifier("^~", ParserTokenType::PT_MOD_PREFIX);
		return *this;
	}

	/**
	 * @brief Registra uma diretiva personalizada.
	 *
	 * @param content Nome da diretiva.
	 * @param type Tipo associado.
	 *
	 * @return Referência para o builder.
	 */
	ParserBuilder	&withDirective(const std::string &content, ParserTokenType::type type)
	{
		_parser.addDirectiveKeyword(content, type);
		return *this;
	}

	/**
	 * @brief Registra um bloco personalizado.
	 *
	 * @param content Nome do bloco.
	 * @param type Tipo associado.
	 *
	 * @return Referência para o builder.
	 */
	ParserBuilder	&withBlock(const std::string &content, ParserTokenType::type type)
	{
		_parser.addBlockKeyword(content, type);
		return *this;
	}

	/**
	 * @brief Registra um modificador personalizado.
	 *
	 * @param content Texto do modificador.
	 * @param type Tipo associado.
	 *
	 * @return Referência para o builder.
	 */
	ParserBuilder	&withModifier(const std::string &content, ParserTokenType::type type)
	{
		_parser.addModifier(content, type);
		return *this;
	}

	/**
	 * @brief Define o arquivo que será analisado.
	 *
	 * Atualiza a configuração do lexer interno para utilizar
	 * o arquivo informado.
	 *
	 * @param file_name Caminho do arquivo.
	 *
	 * @return Referência para o builder.
	 */
	ParserBuilder	&withFile(const std::string &file_name)
	{
		_lexer_builder.withFile(file_name);
		_parser.setLexer(_lexer_builder);
		return *this;
	}
};

#endif
