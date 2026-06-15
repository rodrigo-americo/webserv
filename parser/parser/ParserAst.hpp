/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserAst.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/31 18:46:01 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/04 18:05:00 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_AST_HPP
# define PARSER_AST_HPP

# include "ParserComposite.hpp"
# include "ParserVisitorPrinter.hpp"
# include "ParserVisitorValidator.hpp"

/**
 * @brief Representa a árvore sintática abstrata (AST) produzida pelo parser.
 *
 * Armazena o nó raiz da árvore e os erros encontrados durante
 * as etapas de parsing e validação estrutural.
 *
 * A classe também fornece mecanismos para percorrer a árvore
 * utilizando visitors especializados.
 */
class ParserAst
{
private:
	Block	*_root; // @brief Nó raiz da árvore sintática.
	std::vector<std::string>	_errors; // @brief Lista de erros associados à AST.
public:
	/**
	 * @brief Constrói uma AST com um nó raiz definido.
	 *
	 * @param root Nó raiz da árvore.
	 */
	ParserAst(Block *root): _root(root) {};
	// @brief Constrói uma AST vazia.
	ParserAst(): _root(NULL) {};
	~ParserAst() { if (_root) delete _root; };

	/**
	 * @brief Exibe a árvore sintática.
	 *
	 * Utiliza `ParserVisitorPrinter` para percorrer e imprimir
	 * toda a estrutura da AST.
	 */
	void	print() const
	{
		if (!_root) return;
		ParserVisitorPrinter	printer;
		_root->accept(printer);
	}

	/**
	 * @brief Executa a validação estrutural da árvore.
	 *
	 * Utiliza ParserVisitorValidator para verificar regras
	 * sintáticas e semânticas da estrutura construída.
	 *
	 * Os erros encontrados são adicionados à lista interna
	 * de erros da AST.
	 */
	void	validateStructure()
	{
		if (!_root) return;
		ParserVisitorValidator	validator;
		_root->accept(validator);
		const std::vector<std::string>	&errors = validator.getErrors();
		for (size_t i = 0; i < errors.size(); i++)
			addError(errors[i]);
	}

	/**
	 * @brief Verifica se existem erros registrados.
	 *
	 * @return true se houver pelo menos um erro.
	 */
	bool	hasError() const { return !_errors.empty(); }

	// @brief Exibe todos os erros registrados.
	void	printErrors() const
	{
		for (size_t i = 0; i < _errors.size(); i++)
			std::cout << _errors[i] << "\n";
	}

	/**
	 * @brief Define um novo nó raiz.
	 *
	 * Caso já exista uma raiz armazenada, ela é destruída
	 * antes da substituição.
	 *
	 * @param root Novo nó raiz.
	 */
	void	setRoot(Block *root)
	{
		if (_root)
			delete _root;
		_root = root;
	}

	/**
	 * @brief Adiciona um erro à lista interna.
	 *
	 * @param error Mensagem de erro.
	 */
	void	addError(const std::string &error) { _errors.push_back(error); }

	/**
	 * @brief Aplica um visitor externo na raiz da AST.
	 *
	 * @param visitor Visitor a ser executado.
	 */
	void	applyVisitor(ParserVisitorBase &visitor)
	{
		if (_root)
			_root->accept(visitor);
	}
};

#endif
