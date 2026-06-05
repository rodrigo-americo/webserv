/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserVisitorBase.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/31 15:42:20 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/04 18:06:39 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_VISITOR_BASE_HPP
# define PARSER_VISITOR_BASE_HPP

struct Directive;
struct Block;

/**
 * @brief Interface base para visitors da AST.
 *
 * Define o contrato utilizado pelo padrão Visitor para
 * percorrer e processar os diferentes tipos de nós da
 * árvore sintática.
 *
 * Classes derivadas podem implementar operações como:
 * - impressão da árvore
 * - validação estrutural
 * - geração de código
 * - análise semântica
 */
struct ParserVisitorBase
{
	virtual ~ParserVisitorBase() {};

	/**
	 * @brief Processa um nó do tipo Directive.
	 *
	 * Chamado quando o visitor visita uma diretiva.
	 *
	 * @param directive Diretiva visitada.
	 */
	virtual void	visit(Directive&) = 0;

	/**
	 * @brief Processa um nó do tipo Block.
	 *
	 * Chamado quando o visitor visita um bloco.
	 *
	 * @param block Bloco visitado.
	 */
	virtual void	visit(Block&) = 0;
};

#endif
