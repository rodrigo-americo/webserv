/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 20:23:50 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/05/31 14:56:45 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Lexer.hpp"
#include "LexerBuilder.hpp"

int main()
{
	LexerIterator	lexer = LexerBuilder().withFile("example.config").withDefaultTokens().build();
	if (!lexer.error().empty())
	{
		std::cout << lexer.error() << "\n";
		return 1;
	}
	lexer.show();
	return 0;
}
