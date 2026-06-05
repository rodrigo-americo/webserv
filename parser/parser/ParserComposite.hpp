/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserComposite.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/31 15:26:33 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/04 17:42:39 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_COMPOSITE_HPP
# define PARSER_COMPOSITE_HPP

# include <vector>

# include "ParserVisitorBase.hpp"
# include "ParserToken.hpp"

// @brief Tipos de nós da AST do parser.
enum ParserNodeType
{
	/**
	 * @brief Representa uma diretiva simples.
	 *
	 * Exemplo:
	 * listen 8080;
	 */
	DIRECTIVE,
	/**
	 * @brief Representa um bloco que pode conter outros nós.
	 *
	 * Exemplo:
	 * server { ... }
	 */
	BLOCK
};

/**
 * @brief Nó base da árvore sintática (AST).
 *
 * Define a interface comum para diretivas e blocos.
 * Cada nó possui um nome, uma lista de valores e um tipo.
 */
struct ParserNode
{
	ParserToken					name; // @brief Token que identifica o nome do nó.
	std::vector<ParserToken>	values; // @brief Argumentos associados ao nó.
	ParserNodeType				node_type; // @brief Tipo concreto do nó.

	/**
	 * @brief Aceita a visita de um visitor.
	 *
	 * Implementa o padrão Visitor.
	 *
	 * @param visitor Visitor que processará o nó.
	 */
	virtual void	accept(ParserVisitorBase&) = 0;

	/**
	 * @brief Constrói um nó sem argumentos.
	 *
	 * @param _name Token de identificação.
	 * @param type Tipo do nó.
	 */
	ParserNode(const ParserToken &_name, ParserNodeType type): name(_name), values(), node_type(type) {}

	/**
	 * @brief Constrói um nó com argumentos.
	 *
	 * @param _name Token de identificação.
	 * @param _values Lista de argumentos.
	 * @param type Tipo do nó.
	 */
	ParserNode(const ParserToken &_name, const std::vector<ParserToken> &_values, ParserNodeType type): name(_name), values(_values), node_type(type) {}
	virtual ~ParserNode() {};
};


/**
 * @brief Nó que representa uma diretiva simples.
 *
 * Não possui filhos e normalmente corresponde a uma instrução
 * terminada por ';'.
 */
struct Directive: public ParserNode
{
	/**
	 * @brief Constrói uma diretiva sem argumentos.
	 *
	 * @param _name Nome da diretiva.
	 */
	Directive(const ParserToken &_name): ParserNode(_name, DIRECTIVE) {}

	/**
	 * @brief Constrói uma diretiva com argumentos.
	 *
	 * @param _name Nome da diretiva.
	 * @param _values Argumentos da diretiva.
	 */
	Directive(const ParserToken &_name, const std::vector<ParserToken> &_values): ParserNode(_name, _values, DIRECTIVE) {}

	/**
	 * @brief Permite que um visitor processe a diretiva.
	 *
	 * @param visitor Visitor a ser executado.
	 */
	void	accept(ParserVisitorBase &visitor) { visitor.visit(*this); }
};

/**
 * @brief Nó que representa um bloco.
 *
 * Pode conter outros nós da árvore, formando a estrutura
 * hierárquica da configuração.
 */
struct Block: public ParserNode
{
	std::vector<ParserNode*>	children; // @brief Nós filhos pertencentes ao bloco.

	/**
	 * @brief Constrói um bloco sem argumentos.
	 *
	 * @param _name Nome do bloco.
	 */
	Block(const ParserToken &_name): ParserNode(_name, BLOCK), children() {}

	/**
	 * @brief Constrói um bloco com argumentos.
	 *
	 * @param _name Nome do bloco.
	 * @param _values Argumentos do bloco.
	 */
	Block(const ParserToken &_name, const std::vector<ParserToken> &_values): ParserNode(_name, _values, BLOCK) {}

	/**
	 * @brief Permite que um visitor processe o bloco.
	 *
	 * @param visitor Visitor a ser executado.
	 */
	void	accept(ParserVisitorBase &visitor) { visitor.visit(*this); }
};


#endif
