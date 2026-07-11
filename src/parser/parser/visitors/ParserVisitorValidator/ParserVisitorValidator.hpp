/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserVisitorValidator.hpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 17:28:35 by brunofer          #+#    #+#             */
/*   Updated: 2026/06/06 14:10:08 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_VISITOR_VALIDATOR_HPP
# define PARSER_VISITOR_VALIDATOR_HPP

# include "ScopeValidator.hpp"
# include "ArgAmountValidator.hpp"
# include "ParserVisitorBase.hpp"
# include "ParserComposite.hpp"

/**
 * @brief Visitor responsável pela validação da AST.
 *
 * Percorre toda a árvore sintática verificando:
 *
 * - validade dos escopos dos nós;
 * - quantidade de argumentos de diretivas e blocos;
 * - consistência estrutural da configuração.
 *
 * Os erros encontrados durante a validação são armazenados
 * internamente e podem ser consultados posteriormente.
 */
class ParserVisitorValidator: public ParserVisitorBase
{
private:
	/**
	 * @brief Validador de escopos.
	 *
	 * Responsável por verificar se diretivas e blocos estão
	 * presentes em contextos permitidos.
	 */
	ScopeValidator				_scope_validator;
	/**
	 * @brief Validador de quantidade de argumentos.
	 *
	 * Responsável por verificar se cada nó possui a quantidade
	 * correta de parâmetros.
	 */
	ArgAmountValidator			_arg_amount_validator;
	std::vector<std::string>	_errors; // @brief Lista de erros encontrados durante a validação.

public:
	/**
	 * @brief Constrói um validador utilizando as regras padrão.
	 *
	 * Inicializa automaticamente:
	 *
	 * - ScopeValidator com escopos padrão;
	 *
	 * - ArgAmountValidator com validações padrão.
	 */
	ParserVisitorValidator(): _scope_validator(ScopeValidatorBuilder::defaultScopes()), _arg_amount_validator(ArgAmountValidatorBuilder::defaultValidations()) {};

	/**
	 * @brief Constrói um validador com regras personalizadas.
	 *
	 * Permite utilizar builders configurados externamente.
	 *
	 * @param scope_validator_builder Builder dos escopos.
	 * @param arg_amount_validator_builder Builder das validações
	 * de quantidade de argumentos.
	 */
	ParserVisitorValidator(
		ScopeValidatorBuilder &scope_validator_builder,
		ArgAmountValidatorBuilder &arg_amount_validator_builder)
			: _scope_validator(scope_validator_builder.build()),
			_arg_amount_validator(arg_amount_validator_builder.build()) {};

	~ParserVisitorValidator() {};

	/**
	 * @brief Valida uma diretiva.
	 *
	 * Verifica a quantidade de argumentos da diretiva e
	 * registra possíveis erros.
	 *
	 * Diretivas inválidas ou marcadas como PT_END e PT_WORD
	 * são ignoradas.
	 *
	 * @param directive Diretiva visitada.
	 */
	void	visit(Directive &directive)
	{
		if (directive.name == ParserTokenType::PT_END || directive.name == ParserTokenType::PT_WORD) return;
		_arg_amount_validator.validate(directive);
		_arg_amount_validator.dumpErrorsOn(_errors);
	};

	/**
	 * @brief Valida um bloco e seus descendentes.
	 *
	 * Executa:
	 *
	 * - validação da quantidade de argumentos;
	 *
	 * - validação do escopo dos filhos;
	 *
	 * - percurso recursivo da subárvore.
	 *
	 * O bloco atual é empilhado como escopo ativo antes da
	 * validação dos filhos e removido ao final do processamento.
	 *
	 * @param block Bloco visitado.
	 */
	void	visit(Block &block)
	{
		if (block.name == ParserTokenType::PT_END || block.name == ParserTokenType::PT_WORD) return;
		if (block.name != ParserTokenType::PT_MAIN)
			_arg_amount_validator.validate(block);
		_arg_amount_validator.dumpErrorsOn(_errors);

		_scope_validator.pushScope(block.name.getType());
		_scope_validator.validate(block);
		_scope_validator.dumpErrorsOn(_errors);
		for (size_t i = 0; i < block.children.size(); i++)
			block.children[i]->accept(*this);
		_scope_validator.popScope();
	};

	/**
	 * @brief Obtém os erros encontrados durante a validação.
	 *
	 * @return Referência para a lista de erros.
	 */
	const std::vector<std::string> &getErrors() const { return _errors; }
};

#endif
