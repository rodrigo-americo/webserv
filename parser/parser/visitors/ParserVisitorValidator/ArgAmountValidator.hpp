/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ArgAmountValidator.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 19:59:52 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/04 18:23:28 by bruno-valer      ###   ########.fr       */
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
	typedef std::map<ParserTokenType, schema::detail::integer_base<unsigned> > Validations;
	std::map<ParserTokenType, schema::detail::integer_base<unsigned> >	_validations; // @brief Regras de validação cadastradas.
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
		schema::result<unsigned int> resp = it->second.parse(std::to_string(node.values.size()));
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
	void	addValidation(ParserTokenType type, schema::detail::integer_base<unsigned> validation)
	{
		_validations[type] = validation;
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
		return add(PT_HTTP, schema::uint32().between(0, 0).name("http args amount"))
		.add(PT_EVENTS, schema::uint32().between(0, 0).name("events args amount"))
		.add(PT_SERVER, schema::uint32().between(0, 0).name("server args amount"))
		.add(PT_UPSTREAM, schema::uint32().between(1, 1).name("upstream args amount"))
		.add(PT_LOCATION, schema::uint32().between(1, 2).name("location args amount"))
		.add(PT_WORKER_PROCESSES, schema::uint32().between(1, 1).name("worket_process args amount"))
		.add(PT_WORKER_CONNECTIONS, schema::uint32().between(1, 1).name("worker_connections args amount"))
		.add(PT_ERROR_LOG, schema::uint32().between(1, 2).name("error_log args amount"))
		.add(PT_PID, schema::uint32().between(1, 1).name("pid args amount"))
		.add(PT_LISTEN, schema::uint32().between(1, unlimited).name("listen args amount"))
		.add(PT_SERVER_NAME, schema::uint32().between(1, unlimited).name("server_name args amount"))
		.add(PT_ROOT, schema::uint32().between(1, 1).name("root args amount"))
		.add(PT_INDEX, schema::uint32().between(1, unlimited).name("index args amount"))
		.add(PT_TRY_FILES, schema::uint32().between(2, unlimited).name("try_files args amount"))
		.add(PT_PROXY_PASS, schema::uint32().between(1, 1).name("index args amount"))
		.add(PT_PROXY_SET_HEADER, schema::uint32().between(2, 2).name("proxy_set_header args amount"))
		.add(PT_FASTCGI_PASS, schema::uint32().between(1, 1).name("fast_cgi_pass args amount"))
		.add(PT_FASTCGI_PARAM, schema::uint32().between(2, 2).name("fast_cgi_param args amount"))
		.add(PT_ERROR_PAGE, schema::uint32().between(2, unlimited).name("error_page args amount"))
		.add(PT_ADD_HEADER, schema::uint32().between(2, 3).name("add_header args amount"))
		.add(PT_EXPIRES, schema::uint32().between(1, 1).name("expires args amount"))
		.add(PT_SSL_CERTIFICATE, schema::uint32().between(1, 1).name("ssl_certificate args amount"))
		.add(PT_SSL_CERTIFICATE_KEY, schema::uint32().between(1, 1).name("ssl_certificate_key args amount"))
		.add(PT_SSL_PROTOCOLS, schema::uint32().between(1, unlimited).name("ssl_protocols args amount"))
		.add(PT_SSL_CIPHERS, schema::uint32().between(1, 1).name("ssl_ciphers args amount"))
		.add(PT_CLIENT_MAX_BODY_SIZE, schema::uint32().between(1, 1).name("client_max_body_size args amount"))
		.add(PT_LOG_FORMAT, schema::uint32().between(2, unlimited).name("log_format args amount"))
		.add(PT_ACCESS_LOG, schema::uint32().between(1, 2).name("access_log args amount"))
		.add(PT_RETURN, schema::uint32().between(1, 2).name("return args amount"))

		.add(PT_USE, schema::uint32().between(1, 1).name("use args amount"))
		.add(MULTI_ACCEPT, schema::uint32().between(1, 1).name("multi_accept args amount"))
		.add(PT_INCLUDE, schema::uint32().between(1, unlimited).name("include args amount"))
		.add(DEFAULT_TYPE, schema::uint32().between(1, 1).name("default_type args amount"))
		.add(SENDFILE, schema::uint32().between(1, 1).name("sendfile args amount"))
		.add(KEEPALIVE_TIMEOUT, schema::uint32().between(1, 2).name("keepalive_timeout args amount"))
		.add(PT_SERVER_DIRECTIVE, schema::uint32().between(1, unlimited).name("upstream server args amount"))
		.add(PT_AUTOINDEX, schema::uint32().between(1, 1).name("autoindex args amount"))
		.add(PT_LOG_NOT_FOUND, schema::uint32().between(1, 1).name("log_not_found args amount"))
		.add(PT_PROXY_CACHE_BYPASS, schema::uint32().between(1, unlimited).name("proxy_cache_bypass args amount"))
		.add(PT_FASTCGI_INDEX, schema::uint32().between(1, 1).name("fastcgi_index args amount"));
	}

	/**
	 * @brief Adiciona uma validação personalizada.
	 *
	 * @param type Tipo do nó validado.
	 * @param validation Regra utilizada para validação.
	 *
	 * @return Referência para o builder.
	 */
	ArgAmountValidatorBuilder	&add(ParserTokenType type, schema::detail::integer_base<unsigned> validation)
	{
		_validator.addValidation(type, validation);
		return *this;
	}
};

#endif
