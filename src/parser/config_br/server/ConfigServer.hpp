/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 19:24:37 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/12 12:43:23 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_SERVER_HPP
# define CONFIG_SERVER_HPP

# include <string>
# include <vector>

# include "ConfigServerListen.hpp"
# include "ConfigServerLog.hpp"
# include "ConfigServerSsl.hpp"
# include "ConfigServerErrorPage.hpp"

struct ConfigServer
{
	typedef ConfigServerListen		Listen;
	typedef ConfigServerLog			Log;
	typedef ConfigServerSsl			Ssl;
	typedef ConfigServerErrorPage	ErrorPage;

	std::vector<std::string>	server_name;
	std::string					root;
	std::vector<std::string>	index;
	bool						autoindex;
	std::vector<Listen>			listeners;

	size_t						client_max_body_size;
	size_t						client_body_timeout;
	size_t						client_header_timeout;
	size_t						send_timeout;
	size_t						keepalive_timeout;

	Log							acces_log;
	Log							error_log;

	std::vector<ErrorPage>		error_pages;

	Ssl							ssl;

	ConfigServer()
		: server_name(), root(), index(), autoindex(), listeners(),
			client_max_body_size(1 * 1024 * 1024) /*1m padrão*/, client_body_timeout(60),
			client_header_timeout(60), send_timeout(60), keepalive_timeout(75),
			acces_log(), error_log(), error_pages(), ssl() {};
	~ConfigServer() {};
};

#endif
