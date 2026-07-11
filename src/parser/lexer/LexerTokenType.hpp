/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LexerTokenType.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/31 11:51:17 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/19 19:44:53 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_TOKEN_TYPE_HPP
# define LEXER_TOKEN_TYPE_HPP

# include <iostream>

struct LexerTokenType
{
	enum type
	{
		COMMENT,
		LBRACE,
		RBRACE,
		SEMICOLON,
		STRING_SINGLE,
		STRING_DOUBLE,
		WORD,
		ERROR,
		END,
		DELIM_PLUS,
		DELIM_MINUS,
		DELIM_SEMI,
		DELIM_ASSIGN,
		DELIM_LBRACE,
		DELIM_RBRACE,
		DELIM_ARROW
	};
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
	"DELIM_PLUS",
	"DELIM_MINUS",
	"DELIM_SEMI",
	"DELIM_ASSIGN",
	"DELIM_LBRACE",
	"DELIM_RBRACE",
	"DELIM_ARROW"
};

inline std::ostream	&operator<<(std::ostream &os, LexerTokenType::type type)
{
	os << LexerTokenTypeStr[type];
	return os;
}

#endif
