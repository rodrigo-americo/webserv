/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/30 20:23:50 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/19 20:16:55 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Lexer.hpp"
#include "LexerBuilder.hpp"

int main(int argc, char **argv)
{
	std::string file = "example.config";
	if (argc >= 2)
		file = argv[1];
	LexerIterator	lexer = LexerBuilder().withFile(file).withDefaultTokens().build();
	if (!lexer.error().empty())
	{
		std::cout << lexer.error() << "\n";
		return 1;
	}
	lexer.show();
	return 0;
}
