/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ScopeValidator.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 16:47:55 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/07/06 19:14:18 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCOPE_VALIDATOR_HPP
# define SCOPE_VALIDATOR_HPP

# include <map>
# include <vector>
# include <algorithm>

# include "ParserTokenType.hpp"
# include "ParserComposite.hpp"

/**
 * @brief Validador de escopos da AST.
 *
 * Verifica se diretivas e blocos aparecem em contextos válidos
 * de acordo com as regras previamente configuradas.
 *
 * Exemplo:
 * - listen só pode existir dentro de server.
 * - location só pode existir dentro de server ou location.
 * - worker_processes só pode existir no escopo principal.
 */
class ScopeValidator
{
	private:
		/**
		 * @brief Pilha de escopos atualmente ativos.
		 *
		 * O último elemento representa o escopo atual.
		 */
		std::vector<ParserTokenType::type>	_curr_scopes;
		/**
		 * @brief Tipo utilizado para armazenar regras de validação.
		 *
		 * Mapeia um token para a lista de escopos onde ele é permitido.
		 */
		typedef std::map<ParserTokenType::type, std::vector<ParserTokenType::type> > Validations;
		std::map<ParserTokenType::type, std::vector<ParserTokenType::type> >	_directive_scope; // @brief Regras de escopo para diretivas.
		std::map<ParserTokenType::type, std::vector<ParserTokenType::type> >	_block_scope; // @brief Regras de escopo para blocos.
		std::vector<std::string>	_errors; // @brief Lista de erros encontrados durante a validação.

		/**
		 * @brief Converte uma lista de tipos em texto legível.
		 *
		 * Utilizado na construção de mensagens de erro.
		 *
		 * @param scopes Escopos permitidos.
		 * @return Lista formatada dos escopos.
		 */
		static std::string	_getStringTypes(std::vector<ParserTokenType::type> scopes)
		{
			std::string resp;
			for (size_t i = 0; i < scopes.size(); i++)
			{
				resp += ParserTokenTypeStr[scopes[i]];
				if (i < scopes.size() - 1)
					resp += ", ";
			}
			return resp;
		}

		/**
		 * @brief Valida um nó utilizando um conjunto de regras.
		 *
		 * Verifica se o tipo informado pode existir dentro do
		 * escopo atualmente ativo.
		 *
		 * @param name Nome do nó analisado.
		 * @param validator Conjunto de regras aplicável.
		 */
		void	_validateScopeFrom(ParserToken	&name, Validations &validator)
		{
			if (name == ParserTokenType::PT_END || name == ParserTokenType::PT_WORD) return;
			Validations::iterator validator_it = validator.find(name.getType());
			if (validator_it == validator.end())
			{
				std::string msg = name.getContent() + " " + ParserTokenTypeStr[name.getType()] + " scope not found.";
				_errors.push_back(msg);
				return;
			}
			std::vector<ParserTokenType::type> &scopes = validator_it->second;
			std::vector<ParserTokenType::type>::iterator scope_it = std::find(scopes.begin(), scopes.end(), _curr_scopes.back());
			if (scope_it == scopes.end())
			{
				std::string msg = name.getLineAddress() + " " + name.getContent() + " in wrong context. Must be in scopes '" + _getStringTypes(scopes) + "'";
				_errors.push_back(msg);
			}
		}

	public:
		// @brief Constrói um validador vazio.
		ScopeValidator(): _curr_scopes(), _directive_scope(), _block_scope() {};
		~ScopeValidator() {};

		/**
		 * @brief Valida os filhos de um bloco.
		 *
		 * Verifica se cada diretiva e bloco filho está presente
		 * em um escopo permitido.
		 *
		 * @param block Bloco a ser validado.
		 */
		void	validate(Block &block)
		{
			for (size_t i = 0; i < block.children.size(); i++)
			{
				if (block.children[i]->node_type == BLOCK)
					_validateScopeFrom(block.children[i]->name, _block_scope);
				else
					_validateScopeFrom(block.children[i]->name, _directive_scope);
			}

		}

		/**
		 * @brief Adiciona um novo escopo à pilha.
		 *
		 * @param scope Escopo ativado.
		 */
		void	pushScope(ParserTokenType::type scope)	{ _curr_scopes.push_back(scope); }

		// @brief Remove o escopo atual da pilha.
		void	popScope()							{ _curr_scopes.pop_back(); }

		/**
		 * @brief Registra uma regra para diretivas.
		 *
		 * @param directive Diretiva validada.
		 * @param scope Escopo permitido.
		 */
		void addDirectiveScopeValidation(ParserTokenType::type directive, ParserTokenType::type scope)
		{
			_directive_scope[directive].push_back(scope);
		}

		/**
		 * @brief Registra uma regra para blocos.
		 *
		 * @param block Bloco validado.
		 * @param scope Escopo permitido.
		 */
		void addBlockScopeValidation(ParserTokenType::type block, ParserTokenType::type scope)
		{
			_block_scope[block].push_back(scope);
		}

		/**
		 * @brief Move os erros acumulados para outro vetor.
		 *
		 * Após a operação a lista interna é limpa.
		 *
		 * @param destine Destino dos erros.
		 */
		void	dumpErrorsOn(std::vector<std::string> &destine)
		{
			for (size_t i = 0; i < _errors.size(); i++)
				destine.push_back(_errors[i]);
			_errors.clear();
		}
};

/**
 * @brief Builder para configuração de ScopeValidator.
 *
 * Permite registrar regras de escopo através de uma interface
 * fluente (method chaining).
 */
class ScopeValidatorBuilder
{
	private:
		ScopeValidator	_scope_validator; // @brief Validador em construção.
		/**
		 * @brief Último token configurado.
		 *
		 * Utilizado pelo método andOn().
		 */
		ParserTokenType::type	_last_scope_updated;
		bool			_last_scope_updated_is_block; // @brief Indica se o último token configurado é um bloco.

	public:
		// @brief Constrói um builder vazio.
		ScopeValidatorBuilder()
			: _scope_validator(), _last_scope_updated(), _last_scope_updated_is_block(false){};
		~ScopeValidatorBuilder() {};

		/**
		 * @brief Cria um validador com as regras padrão.
		 *
		 * Configura automaticamente os escopos suportados
		 * pelo parser compatível com Nginx.
		 *
		 * @return Validador configurado.
		 */
		static ScopeValidator	defaultScopes()
		{
			ScopeValidatorBuilder	builder;
			builder.withDefaultBlockScopes();
			builder.withDefaultDirectiveScopes();
			return builder.build();
		}

		/**
		 * @brief Finaliza a construção do validador.
		 *
		 * @return ScopeValidator configurado.
		 */
		ScopeValidator	build() { return _scope_validator; }

		/**
		 * @brief Registra as regras padrão para diretivas.
		 *
		 * @return Referência para o builder.
		 */
		ScopeValidatorBuilder	&withDefaultDirectiveScopes()
		{
			return withDirectiveOnScope(ParserTokenType::PT_WORKER_PROCESSES, ParserTokenType::PT_MAIN)
				.withDirectiveOnScope(ParserTokenType::PT_PID, ParserTokenType::PT_MAIN)
				.withDirectiveOnScope(ParserTokenType::PT_WORKER_PROCESSES, ParserTokenType::PT_MAIN)
				.withDirectiveOnScope(ParserTokenType::PT_INCLUDE, ParserTokenType::PT_MAIN).andOn(ParserTokenType::PT_HTTP).andOn(ParserTokenType::PT_SERVER).andOn(ParserTokenType::PT_LOCATION).andOn(ParserTokenType::PT_UPSTREAM)
					.andOn(ParserTokenType::PT_EVENTS).andOn(ParserTokenType::PT_STREAM).andOn(ParserTokenType::PT_GEO).andOn(ParserTokenType::PT_MAP).andOn(ParserTokenType::PT_TYPES).andOn(ParserTokenType::PT_LIMIT_EXCEPT)
				.withDirectiveOnScope(ParserTokenType::DEFAULT_TYPE, ParserTokenType::PT_HTTP)
				.withDirectiveOnScope(ParserTokenType::SENDFILE, ParserTokenType::PT_HTTP)
				.withDirectiveOnScope(ParserTokenType::KEEPALIVE_TIMEOUT, ParserTokenType::PT_HTTP)
				.withDirectiveOnScope(ParserTokenType::PT_LOG_FORMAT, ParserTokenType::PT_HTTP)
				.withDirectiveOnScope(ParserTokenType::PT_ERROR_LOG, ParserTokenType::PT_MAIN).andOn(ParserTokenType::PT_HTTP).andOn(ParserTokenType::PT_SERVER)
				.withDirectiveOnScope(ParserTokenType::PT_CLIENT_MAX_BODY_SIZE, ParserTokenType::PT_HTTP).andOn(ParserTokenType::PT_SERVER)
				.withDirectiveOnScope(ParserTokenType::PT_USE, ParserTokenType::PT_EVENTS)
				.withDirectiveOnScope(ParserTokenType::MULTI_ACCEPT, ParserTokenType::PT_EVENTS)
				.withDirectiveOnScope(ParserTokenType::PT_WORKER_CONNECTIONS, ParserTokenType::PT_EVENTS)
				.withDirectiveOnScope(ParserTokenType::PT_SERVER_DIRECTIVE, ParserTokenType::PT_UPSTREAM)
				.withDirectiveOnScope(ParserTokenType::PT_RETURN, ParserTokenType::PT_SERVER).andOn(ParserTokenType::PT_LOCATION)
				.withDirectiveOnScope(ParserTokenType::PT_REWRITE, ParserTokenType::PT_SERVER)
				.withDirectiveOnScope(ParserTokenType::PT_LISTEN, ParserTokenType::PT_SERVER)
				.withDirectiveOnScope(ParserTokenType::PT_SERVER_NAME, ParserTokenType::PT_SERVER)
				.withDirectiveOnScope(ParserTokenType::PT_SSL_CERTIFICATE, ParserTokenType::PT_SERVER)
				.withDirectiveOnScope(ParserTokenType::PT_SSL_CERTIFICATE_KEY, ParserTokenType::PT_SERVER)
				.withDirectiveOnScope(ParserTokenType::PT_SSL_PROTOCOLS, ParserTokenType::PT_SERVER)
				.withDirectiveOnScope(ParserTokenType::PT_SSL_CIPHERS, ParserTokenType::PT_SERVER)
				.withDirectiveOnScope(ParserTokenType::PT_PROXY_PASS, ParserTokenType::PT_LOCATION)
				.withDirectiveOnScope(ParserTokenType::PT_FASTCGI_PASS, ParserTokenType::PT_LOCATION)
				.withDirectiveOnScope(ParserTokenType::PT_CGI_EXTENSION, ParserTokenType::PT_LOCATION)
				.withDirectiveOnScope(ParserTokenType::PT_EXPIRES, ParserTokenType::PT_LOCATION) // TODO: expandir para PT_HTTP e PT_SERVER quando HttpConfig e ServerConfig suportarem expires
				.withDirectiveOnScope(ParserTokenType::PT_ROOT, ParserTokenType::PT_LOCATION).andOn(ParserTokenType::PT_HTTP).andOn(ParserTokenType::PT_SERVER)
				.withDirectiveOnScope(ParserTokenType::PT_INDEX, ParserTokenType::PT_LOCATION).andOn(ParserTokenType::PT_HTTP).andOn(ParserTokenType::PT_SERVER)
				.withDirectiveOnScope(ParserTokenType::PT_ACCESS_LOG, ParserTokenType::PT_LOCATION).andOn(ParserTokenType::PT_HTTP).andOn(ParserTokenType::PT_SERVER)
				.withDirectiveOnScope(ParserTokenType::PT_ERROR_PAGE, ParserTokenType::PT_LOCATION).andOn(ParserTokenType::PT_HTTP).andOn(ParserTokenType::PT_SERVER)
				.withDirectiveOnScope(ParserTokenType::PT_ADD_HEADER, ParserTokenType::PT_LOCATION).andOn(ParserTokenType::PT_HTTP).andOn(ParserTokenType::PT_SERVER)
				.withDirectiveOnScope(ParserTokenType::PT_AUTOINDEX, ParserTokenType::PT_LOCATION).andOn(ParserTokenType::PT_HTTP).andOn(ParserTokenType::PT_SERVER)
				.withDirectiveOnScope(ParserTokenType::PT_LOG_NOT_FOUND, ParserTokenType::PT_LOCATION).andOn(ParserTokenType::PT_HTTP).andOn(ParserTokenType::PT_SERVER)
				.withDirectiveOnScope(ParserTokenType::PT_PROXY_SET_HEADER, ParserTokenType::PT_LOCATION).andOn(ParserTokenType::PT_HTTP).andOn(ParserTokenType::PT_SERVER)
				.withDirectiveOnScope(ParserTokenType::PT_FASTCGI_INDEX, ParserTokenType::PT_LOCATION).andOn(ParserTokenType::PT_HTTP).andOn(ParserTokenType::PT_SERVER)
				.withDirectiveOnScope(ParserTokenType::PT_FASTCGI_PARAM, ParserTokenType::PT_LOCATION).andOn(ParserTokenType::PT_HTTP).andOn(ParserTokenType::PT_SERVER)
				.withDirectiveOnScope(ParserTokenType::PT_CGI_EXTENSION, ParserTokenType::PT_LOCATION)
				.withDirectiveOnScope(ParserTokenType::PT_PROXY_CACHE_BYPASS, ParserTokenType::PT_LOCATION).andOn(ParserTokenType::PT_HTTP).andOn(ParserTokenType::PT_SERVER)
				.withDirectiveOnScope(ParserTokenType::PT_TRY_FILES, ParserTokenType::PT_LOCATION).andOn(ParserTokenType::PT_SERVER)
				.withDirectiveOnScope(ParserTokenType::PT_UPLOAD_DIR, ParserTokenType::PT_LOCATION)
				.withDirectiveOnScope(ParserTokenType::PT_ALLOW_METHODS, ParserTokenType::PT_LOCATION)
				.withDirectiveOnScope(ParserTokenType::PT_REQUIRE_AUTH, ParserTokenType::PT_LOCATION);
		}

		/**
		 * @brief Registra as regras padrão para blocos.
		 *
		 * @return Referência para o builder.
		 */
		ScopeValidatorBuilder	&withDefaultBlockScopes()
		{
			return withBlockOnScope(ParserTokenType::PT_EVENTS, ParserTokenType::PT_MAIN)
				.withBlockOnScope(ParserTokenType::PT_HTTP, ParserTokenType::PT_MAIN)
				.withBlockOnScope(ParserTokenType::PT_UPSTREAM, ParserTokenType::PT_HTTP)
				.withBlockOnScope(ParserTokenType::PT_SERVER, ParserTokenType::PT_HTTP)
				.withBlockOnScope(ParserTokenType::PT_LOCATION, ParserTokenType::PT_SERVER).andOn(ParserTokenType::PT_LOCATION);
		}

		/**
		 * @brief Registra uma diretiva em um escopo permitido.
		 *
		 * @param directive Diretiva validada.
		 * @param scope Escopo permitido.
		 *
		 * @return Referência para o builder.
		 */
		ScopeValidatorBuilder	&withDirectiveOnScope(ParserTokenType::type directive, ParserTokenType::type scope)
		{
			_last_scope_updated = directive;
			_last_scope_updated_is_block = false;
			_scope_validator.addDirectiveScopeValidation(directive, scope);
			return *this;
		}

		/**
		 * @brief Registra um bloco em um escopo permitido.
		 *
		 * @param block Bloco validado.
		 * @param scope Escopo permitido.
		 *
		 * @return Referência para o builder.
		 */
		ScopeValidatorBuilder	&withBlockOnScope(ParserTokenType::type block, ParserTokenType::type scope)
		{
			_last_scope_updated = block;
			_last_scope_updated_is_block = true;
			_scope_validator.addBlockScopeValidation(block, scope);
			return *this;
		}

		/**
		 * @brief Adiciona mais um escopo permitido para a última regra.
		 *
		 * Exemplo:
		 *
		 * @code
		 * withDirectiveOnScope(ParserTokenType::PT_ROOT, ParserTokenType::PT_HTTP)
		 *     .andOn(ParserTokenType::PT_SERVER)
		 *     .andOn(ParserTokenType::PT_LOCATION);
		 * @endcode
		 *
		 * @param scope Novo escopo permitido.
		 *
		 * @return Referência para o builder.
		 */
		ScopeValidatorBuilder	&andOn(ParserTokenType::type scope)
		{
			if (_last_scope_updated_is_block)
				_scope_validator.addBlockScopeValidation(_last_scope_updated, scope);
			else
				_scope_validator.addDirectiveScopeValidation(_last_scope_updated, scope);
			return *this;
		}
};

#endif
