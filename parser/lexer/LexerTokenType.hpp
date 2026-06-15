/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LexerTokenType.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/31 11:51:17 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/04 17:36:03 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_TOKEN_TYPE_HPP
# define LEXER_TOKEN_TYPE_HPP

# include <iostream>

enum LexerTokenType
{
	COMMENT			= '#',
	LBRACE			= '{',
	RBRACE			= '}',
	SEMICOLON		= ';',
	STRING_SINGLE	= '\'',
	STRING_DOUBLE	= '"',
	WORD			= 'w',
	ERROR			= 'e',
	END				= '\0',
};

static const char* LexerTokenTypeStr[] =
{
	"COMMENT",
	"LBRACE",
	"RBRACE",
	"SEMICOLON",
	"STRING_SINGLE",
	"STRING_DOUBLE",
	"WORD",
	"ERROR",
	"END",
};

inline std::ostream	&operator<<(std::ostream &os, LexerTokenType type)
{
	os << LexerTokenTypeStr[type];
	return os;
}

#endif
