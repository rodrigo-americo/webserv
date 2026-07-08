/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ArgAmountValidator.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 19:59:52 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/07/08 14:17:30 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ARG_AMOUNT_VALIDATOR_HPP
# define ARG_AMOUNT_VALIDATOR_HPP

# include <map>
# include <vector>
# include <string>
# include <limits>

# include "schema.hpp"
# include "ParserTokenType.hpp"
# include "ParserComposite.hpp"
# include "utils.hpp"

/**
 * @brief Validador da quantidade de argumentos de nós da AST.
 *
 * Verifica se uma diretiva ou bloco possui uma quantidade de
 * argumentos compatível com as regras configuradas.
 *
 * Cada regra é definida através de um schema responsável por
 * validar a quantidade mínima e máxima permitida.
 */
class ArgAmountValidator
{
private:
	/**
	 * @brief Alias para o mapa de validações.
	 *
	 * Associa um tipo de token ao schema responsável por validar
	 * sua quantidade de argumentos.
	 */
	typedef std::map<ParserTokenType::type, schema_unsigned_int > Validations;
	std::map<ParserTokenType::type, schema_unsigned_int >	_validations; // @brief Regras de validação cadastradas.
	std::vector<std::string>	_errors; // @brief Lista de erros encontrados durante a validação.
public:
	// @brief Constrói um validador vazio.
	ArgAmountValidator() {};
	~ArgAmountValidator() {};

	/**
	 * @brief Valida a quantidade de argumentos de um nó.
	 *
	 * Procura a regra correspondente ao tipo do nó e verifica
	 * se o número de argumentos fornecidos é válido.
	 *
	 * Caso não exista regra para o tipo informado ou a
	 * validação falhe, um erro é registrado.
	 *
	 * @param node Nó a ser validado.
	 */
	void	validate(const ParserNode &node)
	{
		Validations::iterator	it = _validations.find(node.name.getType());
		if (it == _validations.end())
		{
			std::string msg = "Type " + std::string(ParserTokenTypeStr[node.name.getType()]);
			msg += " not found on arg_amount_validations.";
			_errors.push_back(msg);
			return;
		}
		schema_result_unsigned_int resp = it->second.parse(utils::to_string(node.values.size()));
		if (!resp)
			for (size_t i = 0; i < resp.errors.size(); i++)
				_errors.push_back(resp.errors[i].format());
	}

	/**
	 * @brief Registra uma nova regra de validação.
	 *
	 * @param type Tipo do nó.
	 * @param validation Schema responsável pela validação.
	 */
	void	addValidation(ParserTokenType::type type, schema::detail::schema_int<unsigned> validation)
	{
		this->_validations[type] = validation;
	}

	/**
	 * @brief Move os erros acumulados para outro vetor.
	 *
	 * Após a operação a lista interna de erros é limpa.
	 *
	 * @param destine Vetor de destino.
	 */
	void	dumpErrorsOn(std::vector<std::string> &destine)
	{
		for (size_t i = 0; i < _errors.size(); i++)
			destine.push_back(_errors[i]);
		_errors.clear();
	}
};

/**
 * @brief Builder responsável pela configuração de
 * ArgAmountValidator.
 *
 * Permite registrar validações individualmente ou utilizar
 * um conjunto padrão compatível com diretivas Nginx.
 */
class ArgAmountValidatorBuilder
{
private:
	ArgAmountValidator	_validator; // @brief Validador em construção.
public:
	// @brief Constrói um builder vazio.
	ArgAmountValidatorBuilder() {};
	~ArgAmountValidatorBuilder() {};

	/**
	 * @brief Cria um validador com as regras padrão.
	 *
	 * Configura automaticamente as validações de quantidade
	 * de argumentos para blocos e diretivas conhecidas.
	 *
	 * @return Validador configurado.
	 */
	static ArgAmountValidator	defaultValidations()
	{
		ArgAmountValidatorBuilder builder;
		return builder.withDefaultValidations().build();
	}

	/**
	 * @brief Finaliza a construção do validador.
	 *
	 * @return Instância configurada de ArgAmountValidator.
	 */
	ArgAmountValidator			build() { return _validator; }

	/**
	 * @brief Registra todas as validações padrão.
	 *
	 * Configura regras para diretivas como:
	 *
	 * - listen
	 *
	 * - server_name
	 *
	 * - root
	 *
	 * - include
	 *
	 * - ssl_protocols
	 *
	 * - proxy_pass
	 *
	 * - entre outras.
	 *
	 * Também configura validações para blocos como:
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
	ArgAmountValidatorBuilder	&withDefaultValidations()
	{
		static unsigned int	unlimited = std::numeric_limits<unsigned>::max();
		return add(ParserTokenType::PT_HTTP, schema::unsigned_integer().between(0, 0).name("http args amount"))
		.add(ParserTokenType::PT_EVENTS, schema::unsigned_integer().between(0, 0).name("events args amount"))
		.add(ParserTokenType::PT_SERVER, schema::unsigned_integer().between(0, 0).name("server args amount"))
		.add(ParserTokenType::PT_UPSTREAM, schema::unsigned_integer().between(1, 1).name("upstream args amount"))
		.add(ParserTokenType::PT_LOCATION, schema::unsigned_integer().between(1, 2).name("location args amount"))
		.add(ParserTokenType::PT_WORKER_PROCESSES, schema::unsigned_integer().between(1, 1).name("worket_process args amount"))
		.add(ParserTokenType::PT_WORKER_CONNECTIONS, schema::unsigned_integer().between(1, 1).name("worker_connections args amount"))
		.add(ParserTokenType::PT_ERROR_LOG, schema::unsigned_integer().between(1, 2).name("error_log args amount"))
		.add(ParserTokenType::PT_PID, schema::unsigned_integer().between(1, 1).name("pid args amount"))
		.add(ParserTokenType::PT_LISTEN, schema::unsigned_integer().between(1, unlimited).name("listen args amount"))
		.add(ParserTokenType::PT_SERVER_NAME, schema::unsigned_integer().between(1, unlimited).name("server_name args amount"))
		.add(ParserTokenType::PT_ROOT, schema::unsigned_integer().between(1, 1).name("root args amount"))
		.add(ParserTokenType::PT_INDEX, schema::unsigned_integer().between(1, unlimited).name("index args amount"))
		.add(ParserTokenType::PT_TRY_FILES, schema::unsigned_integer().between(2, unlimited).name("try_files args amount"))
		.add(ParserTokenType::PT_PROXY_PASS, schema::unsigned_integer().between(1, 1).name("index args amount"))
		.add(ParserTokenType::PT_PROXY_SET_HEADER, schema::unsigned_integer().between(2, 2).name("proxy_set_header args amount"))
		.add(ParserTokenType::PT_FASTCGI_PASS, schema::unsigned_integer().between(1, 1).name("fast_cgi_pass args amount"))
		.add(ParserTokenType::PT_FASTCGI_PARAM, schema::unsigned_integer().between(2, 2).name("fast_cgi_param args amount"))
		.add(ParserTokenType::PT_CGI_EXTENSION, schema::unsigned_integer().between(2, 2).name("cgi_extension args amount"))
		.add(ParserTokenType::PT_ERROR_PAGE, schema::unsigned_integer().between(2, unlimited).name("error_page args amount"))
		.add(ParserTokenType::PT_ADD_HEADER, schema::unsigned_integer().between(2, 3).name("add_header args amount"))
		.add(ParserTokenType::PT_EXPIRES, schema::unsigned_integer().between(1, 1).name("expires args amount"))
		.add(ParserTokenType::PT_SSL_CERTIFICATE, schema::unsigned_integer().between(1, 1).name("ssl_certificate args amount"))
		.add(ParserTokenType::PT_SSL_CERTIFICATE_KEY, schema::unsigned_integer().between(1, 1).name("ssl_certificate_key args amount"))
		.add(ParserTokenType::PT_SSL_PROTOCOLS, schema::unsigned_integer().between(1, unlimited).name("ssl_protocols args amount"))
		.add(ParserTokenType::PT_SSL_CIPHERS, schema::unsigned_integer().between(1, 1).name("ssl_ciphers args amount"))
		.add(ParserTokenType::PT_CLIENT_MAX_BODY_SIZE, schema::unsigned_integer().between(1, 1).name("client_max_body_size args amount"))
		.add(ParserTokenType::PT_LOG_FORMAT, schema::unsigned_integer().between(2, unlimited).name("log_format args amount"))
		.add(ParserTokenType::PT_ACCESS_LOG, schema::unsigned_integer().between(1, 2).name("access_log args amount"))
		.add(ParserTokenType::PT_RETURN, schema::unsigned_integer().between(1, 2).name("return args amount"))

		.add(ParserTokenType::PT_USE, schema::unsigned_integer().between(1, 1).name("use args amount"))
		.add(ParserTokenType::MULTI_ACCEPT, schema::unsigned_integer().between(1, 1).name("multi_accept args amount"))
		.add(ParserTokenType::PT_INCLUDE, schema::unsigned_integer().between(1, unlimited).name("include args amount"))
		.add(ParserTokenType::DEFAULT_TYPE, schema::unsigned_integer().between(1, 1).name("default_type args amount"))
		.add(ParserTokenType::SENDFILE, schema::unsigned_integer().between(1, 1).name("sendfile args amount"))
		.add(ParserTokenType::KEEPALIVE_TIMEOUT, schema::unsigned_integer().between(1, 2).name("keepalive_timeout args amount"))
		.add(ParserTokenType::PT_SERVER_DIRECTIVE, schema::unsigned_integer().between(1, unlimited).name("upstream server args amount"))
		.add(ParserTokenType::PT_AUTOINDEX, schema::unsigned_integer().between(1, 1).name("autoindex args amount"))
		.add(ParserTokenType::PT_LOG_NOT_FOUND, schema::unsigned_integer().between(1, 1).name("log_not_found args amount"))
		.add(ParserTokenType::PT_PROXY_CACHE_BYPASS, schema::unsigned_integer().between(1, unlimited).name("proxy_cache_bypass args amount"))
		.add(ParserTokenType::PT_FASTCGI_INDEX, schema::unsigned_integer().between(1, 1).name("fastcgi_index args amount"))
		.add(ParserTokenType::PT_UPLOAD_DIR, schema::unsigned_integer().between(1, 1).name("upload_dir args amount"))
		.add(ParserTokenType::PT_ALLOW_METHODS, schema::unsigned_integer().between(1, 3).name("allow_methods args amount"))
		.add(ParserTokenType::PT_REQUIRE_AUTH, schema::unsigned_integer().between(1, 1).name("required auth args amount"));
	}

	/**
	 * @brief Adiciona uma validação personalizada.
	 *
	 * @param type Tipo do nó validado.
	 * @param validation Regra utilizada para validação.
	 *
	 * @return Referência para o builder.
	 */
	ArgAmountValidatorBuilder	&add(ParserTokenType::type type, schema::detail::schema_int<unsigned> validation)
	{
		_validator.addValidation(type, validation);
		return *this;
	}
};

#endif
