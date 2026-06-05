/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/31 18:56:11 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/04 12:51:03 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParserBuilder.hpp"
#include "Parser.hpp"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "wrong number of params. try ./" << argv[0] << " <file_name>\n";
		return 1;
	}
	ParserAst	ast = ParserBuilder::defaultBuilder().withFile(argv[1]).build();

	ast.validateStructure();

	if (ast.hasError())
	{
		ast.printErrors();
		return 1;
	}
	ast.print();
	return 0;
}
