/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListenSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 19:38:44 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/08 00:49:10 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LISTEN_SOCKET_HPP
# define LISTEN_SOCKET_HPP

# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/tcp.h>
# include <poll.h>

# include <vector>
# include <string>

# include "utils.hpp"
# include "ConfigServerListen.hpp"
# include "Socket.hpp"

class ListenSocket: public Socket, public ConfigServerListen
{
	private:


	bool _initPollFd()
	{
		_fd.events = POLLIN;

		_fd.fd = socket(_is_unix ? AF_UNIX : AF_INET, SOCK_STREAM, 0);
		if (_fd.fd < 0)
		{
			_errors.push_back(std::string("Error on create listen socket as SOCK_STREAM."));
			return false;
		}

		int enable = 1;
		// isso e apenas para tests (deixa refazer o bind de uma conexao antes do tempo de expiracao):
		if (setsockopt(_fd.fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == - 1)
			_errors.push_back(std::string("Error on set socket option SO_REUSEADDR as ") + utils::to_string(enable == 1));

		if (_keepalive == KeepAlive::ON || _keepalive == KeepAlive::CUSTOM)
			if (setsockopt(_fd.fd, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(enable)) == -1)
				_errors.push_back(std::string("Error on set socket option SO_KEEPALIVE as ") + (enable ? "true" : "false"));
		if (_keepalive == KeepAlive::CUSTOM)
		{
			if (setsockopt(_fd.fd, IPPROTO_TCP, TCP_KEEPIDLE, &_keepalive_time, sizeof(_keepalive_time)) == -1)
				_errors.push_back(std::string("Error on set socket option TCP_KEEPIDLE as ") + utils::to_string(_keepalive_time));
			if (setsockopt(_fd.fd, IPPROTO_TCP, TCP_KEEPINTVL, &_keepalive_intvl, sizeof(_keepalive_intvl)) == -1)
				_errors.push_back(std::string("Error on set socket option TCP_KEEPINTVL as ") + utils::to_string(_keepalive_intvl));
			if (setsockopt(_fd.fd, IPPROTO_TCP, TCP_KEEPCNT, &_keepalive_probes, sizeof(_keepalive_probes)) == -1)
				_errors.push_back(std::string("Error on set socket option TCP_KEEPCNT as ") + utils::to_string(_keepalive_probes));
		}
		return true;
	}

	void	_initAddr()
	{
		// só testando. [Valores arbitrários]...
		_addr.sin_family = AF_INET;
		_addr.sin_port = htons(8080);
		_addr.sin_addr.s_addr = INADDR_ANY;
	}

	public:
		ListenSocket(ConfigServerListen &config_liten): Socket(SocketType::LISTENNER), ConfigServerListen(config_liten)
		{
			if (!_initPollFd()) return;
			_initAddr();
		};
		~ListenSocket() {};
};

#endif
