/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/12 23:59:48 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/13 00:33:31 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "ServerListenParser.hpp"

int main()
{
	// listen [2001:db8::1]:443 default_server ssl http2 proxy_protocol reuseport bind deferred ipv6only=on backlog=2048 rcvbuf=256k sndbuf=256k so_keepalive=30:10:5
	Directive	dir(ParserToken("file_test", 0, 0, "listen", ParserTokenType::PT_LISTEN));
	dir.values.push_back(ParserToken("file_test", 0, 0, "[2001:db8::1]:443", ParserTokenType::PT_WORD));
	dir.values.push_back(ParserToken("file_test", 0, 0, "default_server", ParserTokenType::PT_WORD));
	dir.values.push_back(ParserToken("file_test", 0, 0, "ssl", ParserTokenType::PT_WORD));
	dir.values.push_back(ParserToken("file_test", 0, 0, "http2", ParserTokenType::PT_WORD));
	dir.values.push_back(ParserToken("file_test", 0, 0, "proxy_protocol", ParserTokenType::PT_WORD));
	dir.values.push_back(ParserToken("file_test", 0, 0, "reuseport", ParserTokenType::PT_WORD));
	dir.values.push_back(ParserToken("file_test", 0, 0, "bind", ParserTokenType::PT_WORD));
	dir.values.push_back(ParserToken("file_test", 0, 0, "deferred", ParserTokenType::PT_WORD));
	dir.values.push_back(ParserToken("file_test", 0, 0, "ipv6only=on", ParserTokenType::PT_WORD));
	dir.values.push_back(ParserToken("file_test", 0, 0, "backlog=2048", ParserTokenType::PT_WORD));
	dir.values.push_back(ParserToken("file_test", 0, 0, "rcvbuf=256k", ParserTokenType::PT_WORD));
	dir.values.push_back(ParserToken("file_test", 0, 0, "sndbuf=256k", ParserTokenType::PT_WORD));
	dir.values.push_back(ParserToken("file_test", 0, 0, "so_keepalive=30:10:5", ParserTokenType::PT_WORD));
	ServerListenParser	parser(dir);
	ConfigServer	server_config;
	parser.toConfig(server_config);
	std::cout << "\n\nparsing: listen [2001:db8::1]:443 default_server ssl http2 proxy_protocol reuseport bind deferred ipv6only=on backlog=2048 rcvbuf=256k sndbuf=256k so_keepalive=30:10:5\n\n";
	std::cout << server_config.listeners[0];
}
