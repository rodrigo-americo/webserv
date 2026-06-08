/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 19:24:37 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/07 19:33:27 by bruno-valer      ###   ########.fr       */
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

class ConfigServer
{
	public:
		typedef ConfigServerListen		Listen;
		typedef ConfigServerLog			Log;
		typedef ConfigServerSsl			Ssl;
		typedef ConfigServerErrorPage	ErrorPage;

	private:
		std::vector<std::string>	_server_name;
		std::string					_root;
		std::vector<std::string>	_index;
		bool						_autoindex;
		std::vector<Listen>			_listeners;

		size_t					_client_max_body_size  = 1 * 1024 * 1024; // 1m padrão
		size_t					_client_body_timeout   = 60;
		size_t					_client_header_timeout = 60;
		size_t					_send_timeout          = 60;
		size_t					_keepalive_timeout     = 75;

		Log						_acces_log;
		Log						_error_log;

		std::vector<ErrorPage>	_error_pages;

		Ssl						_ssl;

	public:
		ConfigServer() {};
		~ConfigServer() {};
};

#endif
