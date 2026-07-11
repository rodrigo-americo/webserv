/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserVisitorFreeAst.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 11:45:58 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/06 11:50:40 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_VISITOR_FREE_AST_HPP
# define PARSER_VISITOR_FREE_AST_HPP

# include "ParserVisitorBase.hpp"
# include "ParserComposite.hpp"

class ParserVisitorFreeAst: public ParserVisitorBase
{
private:

public:
	ParserVisitorFreeAst() {};
	~ParserVisitorFreeAst() {};

	void	visit(Directive &directive)
	{
		delete &directive;
	}

	void	visit(Block &block)
	{
		for (size_t i = 0; i < block.children.size(); i++)
			block.children[i]->accept(*this);
		delete &block;
	}

};

#endif
