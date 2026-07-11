/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserVisitorPrinter.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/31 18:28:35 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/06 14:05:39 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_VISITOR_PRINTER_HPP
# define PARSER_VISITOR_PRINTER_HPP

# include <string>
# include <iostream>
# include <sstream>

# include "ParserVisitorBase.hpp"
# include "ParserComposite.hpp"

/**
 * @brief Visitor responsável por imprimir a AST.
 *
 * Percorre a árvore sintática e exibe sua estrutura de forma
 * hierárquica, utilizando indentação para representar o nível
 * de aninhamento dos blocos.
 */
class ParserVisitorPrinter: public ParserVisitorBase
{
private:
	/**
	 * @brief Nível atual de indentação.
	 *
	 * Cada nível corresponde a quatro espaços na saída.
	 */
	size_t	_indent;

	/**
	 * @brief Gera o prefixo de indentação atual.
	 *
	 * @return String contendo os espaços de indentação.
	 */
	std::string	getPrefix() const { return std::string(4 * _indent, ' '); }
public:
	// @brief Constrói um visitor de impressão.
	ParserVisitorPrinter(): _indent(0) {};
	~ParserVisitorPrinter() {};

	/**
	 * @brief Imprime uma diretiva.
	 *
	 * Exibe o nome da diretiva seguido de seus argumentos.
	 *
	 * Exemplo:
	 * listen -> 8080
	 *
	 * @param directive Diretiva visitada.
	 */
	void	visit(Directive &directive)
	{
		if (directive.name == ParserTokenType::PT_END) return;
		std::stringstream	ss;
		ss << getPrefix() << directive.name.getContent() << " -> ";
		for (size_t i = 0; i < directive.values.size(); i++)
		{
			if (directive.values[i] == ParserTokenType::PT_END) return;
			ss << directive.values[i].getContent() << " ";
		}
		ss << "\n";
		std::cout << ss.str();
	}

	/**
	 * @brief Imprime um bloco e seus filhos.
	 *
	 * Exibe o bloco atual e percorre recursivamente todos
	 * os nós filhos aumentando a indentação.
	 *
	 * Exemplo:
	 * [server]
	 *     listen -> 8080
	 *     root -> /var/www
	 *
	 * @param block Bloco visitado.
	 */
	void	visit(Block &block)
	{
		std::cout << getPrefix() << "[" << block.name.getContent() << "] ";
		for (size_t i = 0; i < block.values.size(); i++)
			std::cout << block.values[i].getContent() << " ";
		std::cout << "\n";
		++_indent;
		for (size_t i = 0; i < block.children.size(); i++)
			block.children[i]->accept(*this);
		--_indent;
	}
};

#endif
