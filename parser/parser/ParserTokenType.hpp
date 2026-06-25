/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserTokenType.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunofer <brunofer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/31 11:52:51 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/15 13:42:12 by brunofer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_TOKEN_TYPE_HPP
# define PARSER_TOKEN_TYPE_HPP

# include <iostream>

struct ParserTokenType
{
	enum type
	{
		PT_MAIN,

		// tipos léxicos
		PT_COMMENT,
		PT_LBRACE,
		PT_RBRACE,
		PT_SEMICOLON,
		PT_STRING_SINGLE,
		PT_STRING_DOUBLE,
		PT_WORD,
		PT_ERROR,
		PT_END,

		// blocos
		PT_HTTP,
		PT_EVENTS,
		PT_SERVER,
		PT_LOCATION,
		PT_UPSTREAM,
		PT_GEO,
		PT_MAP,
		PT_STREAM,
		PT_TYPES,
		PT_LIMIT_EXCEPT,

		// diretivas especiais

		PT_PID,
		PT_USE,
		MULTI_ACCEPT,
		DEFAULT_TYPE,
		SENDFILE,
		KEEPALIVE_TIMEOUT,
		PT_WORKER_PROCESSES,
		PT_WORKER_CONNECTIONS,
		PT_LISTEN,
		PT_SERVER_NAME,
		PT_SERVER_DIRECTIVE,
		PT_EXPIRES,
		PT_ROOT,
		PT_LOG_FORMAT,
		PT_LOG_NOT_FOUND,
		PT_ACCESS_LOG,
		PT_ERROR_LOG,
		PT_ERROR_PAGE,
		PT_TRY_FILES,
		PT_ADD_HEADER,
		PT_INDEX,
		PT_AUTOINDEX,
		PT_INCLUDE,
		PT_PROXY_PASS,
		PT_PROXY_CACHE_BYPASS,
		PT_PROXY_SET_HEADER,
		PT_FASTCGI_PASS,
		PT_FASTCGI_PARAM,
		PT_FASTCGI_INDEX,
		PT_RETURN,
		PT_REWRITE,
		PT_SSL_CERTIFICATE,
		PT_SSL_CERTIFICATE_KEY,
		PT_SSL_PROTOCOLS,
		PT_SSL_CIPHERS,
		PT_CLIENT_MAX_BODY_SIZE,
		PT_UPLOAD_DIR,

		// modificadores de location
		PT_MOD_EXACT,        // =
		PT_MOD_REGEX,        // ~
		PT_MOD_REGEX_CI,     // ~*
		PT_MOD_PREFIX,       // ^~

		// valores semânticos
		PT_BOOL_ON,
		PT_BOOL_OFF,
	};
};

static const char* ParserTokenTypeStr[] =
{
	"PT_MAIN",

	// tipos léxicos
	"PT_COMMENT",
	"PT_LBRACE",
	"PT_RBRACE",
	"PT_SEMICOLON",
	"PT_STRING_SINGLE",
	"PT_STRING_DOUBLE",
	"PT_WORD",
	"PT_ERROR",
	"PT_END",

	// blocos
	"PT_HTTP",
	"PT_EVENTS",
	"PT_SERVER",
	"PT_LOCATION",
	"PT_UPSTREAM",
	"PT_GEO",
	"PT_MAP",
	"PT_STREAM",
	"PT_TYPES",
	"PT_LIMIT_EXCEPT",

	// diretivas especiais

	"PT_PID",
	"PT_USE",
	"MULTI_ACCEPT",
	"DEFAULT_TYPE",
	"SENDFILE",
	"KEEPALIVE_TIMEOUT",
	"PT_WORKER_PROCESSES",
	"PT_WORKER_CONNECTIONS",
	"PT_LISTEN",
	"PT_SERVER_NAME",
	"PT_SERVER_DIRECTIVE",
	"PT_EXPIRES",
	"PT_ROOT",
	"PT_LOG_FORMAT",
	"PT_LOG_NOT_FOUND",
	"PT_ACCESS_LOG",
	"PT_ERROR_LOG",
	"PT_ERROR_PAGE",
	"PT_TRY_FILES",
	"PT_ADD_HEADER",
	"PT_INDEX",
	"PT_AUTOINDEX",
	"PT_INCLUDE",
	"PT_PROXY_PASS",
	"PT_PROXY_CACHE_BYPASS",
	"PT_PROXY_SET_HEADER",
	"PT_FASTCGI_PASS",
	"PT_FASTCGI_PARAM",
	"PT_FASTCGI_INDEX",
	"PT_RETURN",
	"PT_REWRITE",
	"PT_SSL_CERTIFICATE",
	"PT_SSL_CERTIFICATE_KEY",
	"PT_SSL_PROTOCOLS",
	"PT_SSL_CIPHERS",
	"PT_CLIENT_MAX_BODY_SIZE",
	"PT_UPLOAD_DIR",

	// modificadores de location
	"PT_MOD_EXACT",        // =
	"PT_MOD_REGEX",        // ~
	"PT_MOD_REGEX_CI",     // ~*
	"PT_MOD_PREFIX",       // ^~

	// valores semânticos
	"PT_BOOL_ON",
	"PT_BOOL_OFF",
};

inline std::ostream	&operator<<(std::ostream &os, ParserTokenType::type type)
{
	os << ParserTokenTypeStr[type];
	return os;
}

#endif
