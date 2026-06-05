/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ScopeValidator.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 16:47:55 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/04 18:16:51 by bruno-valer      ###   ########.fr       */
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
		std::vector<ParserTokenType>	_curr_scopes;
		/**
		 * @brief Tipo utilizado para armazenar regras de validação.
		 *
		 * Mapeia um token para a lista de escopos onde ele é permitido.
		 */
		typedef std::map<ParserTokenType, std::vector<ParserTokenType>> Validations;
		std::map<ParserTokenType, std::vector<ParserTokenType>>	_directive_scope; // @brief Regras de escopo para diretivas.
		std::map<ParserTokenType, std::vector<ParserTokenType>>	_block_scope; // @brief Regras de escopo para blocos.
		std::vector<std::string>	_errors; // @brief Lista de erros encontrados durante a validação.

		/**
		 * @brief Converte uma lista de tipos em texto legível.
		 *
		 * Utilizado na construção de mensagens de erro.
		 *
		 * @param scopes Escopos permitidos.
		 * @return Lista formatada dos escopos.
		 */
		static std::string	_getStringTypes(std::vector<ParserTokenType> scopes)
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
			if (name == PT_END || name == PT_WORD) return;
			Validations::iterator validator_it = validator.find(name.getType());
			if (validator_it == validator.end())
			{
				std::string msg = name.getContent() + " " + ParserTokenTypeStr[name.getType()] + " scope not found.";
				_errors.push_back(msg);
				return;
			}
			std::vector<ParserTokenType> &scopes = validator_it->second;
			std::vector<ParserTokenType>::iterator scope_it = std::find(scopes.begin(), scopes.end(), _curr_scopes[_curr_scopes.size() - 1]);
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
		void	pushScope(ParserTokenType scope)	{ _curr_scopes.push_back(scope); }

		// @brief Remove o escopo atual da pilha.
		void	popScope()							{ _curr_scopes.pop_back(); }

		/**
		 * @brief Registra uma regra para diretivas.
		 *
		 * @param directive Diretiva validada.
		 * @param scope Escopo permitido.
		 */
		void addDirectiveScopeValidation(ParserTokenType directive, ParserTokenType scope)
		{
			_directive_scope[directive].push_back(scope);
		}

		/**
		 * @brief Registra uma regra para blocos.
		 *
		 * @param block Bloco validado.
		 * @param scope Escopo permitido.
		 */
		void addBlockScopeValidation(ParserTokenType block, ParserTokenType scope)
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
		ParserTokenType	_last_scope_updated;
		bool			_last_scope_updated_is_block; // @brief Indica se o último token configurado é um bloco.
		ParserTokenType	_tracking_scope; // @brief Escopo atualmente acompanhado pelo builder.

	public:
		// @brief Constrói um builder vazio.
		ScopeValidatorBuilder()
			: _scope_validator(), _last_scope_updated(), _last_scope_updated_is_block(false), _tracking_scope() {};
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
			return withDirectiveOnScope(PT_WORKER_PROCESSES, PT_MAIN)
				.withDirectiveOnScope(PT_PID, PT_MAIN)
				.withDirectiveOnScope(PT_WORKER_PROCESSES, PT_MAIN)
				.withDirectiveOnScope(PT_INCLUDE, PT_MAIN).andOn(PT_HTTP).andOn(PT_SERVER).andOn(PT_LOCATION).andOn(PT_UPSTREAM)
					.andOn(PT_EVENTS).andOn(PT_STREAM).andOn(PT_GEO).andOn(PT_MAP).andOn(PT_TYPES).andOn(PT_LIMIT_EXCEPT)
				.withDirectiveOnScope(DEFAULT_TYPE, PT_HTTP)
				.withDirectiveOnScope(SENDFILE, PT_HTTP)
				.withDirectiveOnScope(KEEPALIVE_TIMEOUT, PT_HTTP)
				.withDirectiveOnScope(PT_LOG_FORMAT, PT_HTTP)
				.withDirectiveOnScope(PT_ERROR_LOG, PT_MAIN).andOn(PT_HTTP).andOn(PT_SERVER)
				.withDirectiveOnScope(PT_CLIENT_MAX_BODY_SIZE, PT_HTTP).andOn(PT_SERVER)
				.withDirectiveOnScope(PT_USE, PT_EVENTS)
				.withDirectiveOnScope(MULTI_ACCEPT, PT_EVENTS)
				.withDirectiveOnScope(PT_WORKER_CONNECTIONS, PT_EVENTS)
				.withDirectiveOnScope(PT_SERVER_DIRECTIVE, PT_UPSTREAM)
				.withDirectiveOnScope(PT_RETURN, PT_SERVER)
				.withDirectiveOnScope(PT_REWRITE, PT_SERVER)
				.withDirectiveOnScope(PT_LISTEN, PT_SERVER)
				.withDirectiveOnScope(PT_SERVER_NAME, PT_SERVER)
				.withDirectiveOnScope(PT_SSL_CERTIFICATE, PT_SERVER)
				.withDirectiveOnScope(PT_SSL_CERTIFICATE_KEY, PT_SERVER)
				.withDirectiveOnScope(PT_SSL_PROTOCOLS, PT_SERVER)
				.withDirectiveOnScope(PT_SSL_CIPHERS, PT_SERVER)
				.withDirectiveOnScope(PT_PROXY_PASS, PT_LOCATION)
				.withDirectiveOnScope(PT_FASTCGI_PASS, PT_LOCATION)
				.withDirectiveOnScope(PT_EXPIRES, PT_LOCATION)
				.withDirectiveOnScope(PT_ROOT, PT_LOCATION).andOn(PT_HTTP).andOn(PT_SERVER)
				.withDirectiveOnScope(PT_INDEX, PT_LOCATION).andOn(PT_HTTP).andOn(PT_SERVER)
				.withDirectiveOnScope(PT_ACCESS_LOG, PT_LOCATION).andOn(PT_HTTP).andOn(PT_SERVER)
				.withDirectiveOnScope(PT_ERROR_PAGE, PT_LOCATION).andOn(PT_HTTP).andOn(PT_SERVER)
				.withDirectiveOnScope(PT_ADD_HEADER, PT_LOCATION).andOn(PT_HTTP).andOn(PT_SERVER)
				.withDirectiveOnScope(PT_AUTOINDEX, PT_LOCATION).andOn(PT_HTTP).andOn(PT_SERVER)
				.withDirectiveOnScope(PT_LOG_NOT_FOUND, PT_LOCATION).andOn(PT_HTTP).andOn(PT_SERVER)
				.withDirectiveOnScope(PT_PROXY_SET_HEADER, PT_LOCATION).andOn(PT_HTTP).andOn(PT_SERVER)
				.withDirectiveOnScope(PT_FASTCGI_INDEX, PT_LOCATION).andOn(PT_HTTP).andOn(PT_SERVER)
				.withDirectiveOnScope(PT_FASTCGI_PARAM, PT_LOCATION).andOn(PT_HTTP).andOn(PT_SERVER)
				.withDirectiveOnScope(PT_PROXY_CACHE_BYPASS, PT_LOCATION).andOn(PT_HTTP).andOn(PT_SERVER)
				.withDirectiveOnScope(PT_TRY_FILES, PT_LOCATION).andOn(PT_SERVER);
		}

		/**
		 * @brief Registra as regras padrão para blocos.
		 *
		 * @return Referência para o builder.
		 */
		ScopeValidatorBuilder	&withDefaultBlockScopes()
		{
			return withBlockOnScope(PT_EVENTS, PT_MAIN)
				.withBlockOnScope(PT_HTTP, PT_MAIN)
				.withBlockOnScope(PT_UPSTREAM, PT_HTTP)
				.withBlockOnScope(PT_SERVER, PT_HTTP)
				.withBlockOnScope(PT_LOCATION, PT_SERVER).andOn(PT_LOCATION);
		}

		/**
		 * @brief Registra uma diretiva em um escopo permitido.
		 *
		 * @param directive Diretiva validada.
		 * @param scope Escopo permitido.
		 *
		 * @return Referência para o builder.
		 */
		ScopeValidatorBuilder	&withDirectiveOnScope(ParserTokenType directive, ParserTokenType scope)
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
		ScopeValidatorBuilder	&withBlockOnScope(ParserTokenType block, ParserTokenType scope)
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
		 * withDirectiveOnScope(PT_ROOT, PT_HTTP)
		 *     .andOn(PT_SERVER)
		 *     .andOn(PT_LOCATION);
		 * @endcode
		 *
		 * @param scope Novo escopo permitido.
		 *
		 * @return Referência para o builder.
		 */
		ScopeValidatorBuilder	&andOn(ParserTokenType scope)
		{
			if (_last_scope_updated_is_block)
				_scope_validator.addBlockScopeValidation(_last_scope_updated, scope);
			else
				_scope_validator.addDirectiveScopeValidation(_last_scope_updated, scope);
			return *this;
		}
};

#endif
