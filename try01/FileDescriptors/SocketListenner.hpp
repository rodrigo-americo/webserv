/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketListenner.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 22:37:04 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/07/09 16:48:56 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_LISTENNER_HPP
# define SOCKET_LISTENNER_HPP

# include <errno.h>
# include <netinet/in.h>
# include <netinet/tcp.h>
#include <arpa/inet.h>

# include <iostream>

# include "Logger.hpp"
# include "Socket.hpp"
# include "ConfigServerListen.hpp"

class SocketListenner: public Socket
{
	private:
		void	_setSocketOptions(const ConfigServerListen &listenner)
		{
			int enable = 1;
			size_t	size = sizeof(enable);
			if (setsockopt(fd(), SOL_SOCKET, SO_REUSEADDR, &enable, size) < 0)
				_errors.push_back(std::string("Error on set socket option SO_REUSEADDR: ") + strerror(errno));

			if (listenner.reuseport)
				if (setsockopt(fd(), SOL_SOCKET, SO_REUSEPORT, &enable, size) < 0)
					_errors.push_back(std::string("Error on set socket option SO_REUSEPORT: ") + strerror(errno));

			if (listenner.ipv6only)
				if (setsockopt(fd(), IPPROTO_IPV6, IPV6_V6ONLY, &enable, size) < 0)
					_errors.push_back(std::string("Error on set socket option IPV6_V6ONLY: ") + strerror(errno));

			typedef ConfigServerListenKeepAlive	KeepAlive;
			if (listenner.keepalive == KeepAlive::ON || listenner.keepalive == KeepAlive::CUSTOM)
				if (setsockopt(fd(), SOL_SOCKET, SO_KEEPALIVE, &enable, size) < 0)
					_errors.push_back(std::string("Error on set socket option SO_KEEPALIVE: ") + strerror(errno));
			if (listenner.keepalive == KeepAlive::CUSTOM)
			{
				int iddle = listenner.keepalive_time;
				if (setsockopt(fd(), IPPROTO_TCP, TCP_KEEPIDLE, &iddle, sizeof(iddle)) < 0)
					_errors.push_back(std::string("Error on set socket option TCP_KEEPIDLE: ") + strerror(errno));
				int interval = listenner.keepalive_intvl;
				if (setsockopt(fd(), IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(interval)) < 0)
					_errors.push_back(std::string("Error on set socket option TCP_KEEPINTVL: ") + strerror(errno));
				int count = listenner.keepalive_probes;
				if (setsockopt(fd(), IPPROTO_TCP, TCP_KEEPCNT, &count, sizeof(count)) < 0)
					_errors.push_back(std::string("Error on set socket option TCP_KEEPCNT: ") + strerror(errno));
			}
		}

		void	_setAddr(const ConfigServerListen &listenner)
		{
			if (listenner.is_unix)
				_addr.toUnix(listenner.address);
			else if (listenner.is_ipv4)
				_addr.toIpv4(listenner.port, listenner.address);
			else if (listenner.is_ipv6)
				_addr.toIpv6(listenner.port, listenner.address);
			LOG_TRACE("_setAddr: " << _addr.ip() << ":" << _addr.port());
		}

		void	_bind_and_listen(size_t worker_connections)
		{
			char ip_str[INET_ADDRSTRLEN];
			struct in_addr ip = ((struct sockaddr_in *)_addr.ptr())->sin_addr;
			inet_ntop(AF_INET, &ip, ip_str, INET_ADDRSTRLEN);

			LOG_TRACE("_addr.ptr()->sa_data: " << ip_str);
			if (bind(fd(), _addr.ptr(), _addr.size()) < 0)
			{
				_errors.push_back(std::string("Error on bind: ") + strerror(errno));
				printErrors();
				std::cerr << std::endl;
				return;
			}
			if (listen(fd(), worker_connections) < 0)
			{
				_errors.push_back(std::string("Error on linten: ") + strerror(errno));
				printErrors();
				std::cerr << std::endl;
				return;
			}
		}

	public:
		SocketListenner(const ConfigServerListen &listenner, size_t worker_connections): Socket(FileDescriptorType::SOCKET_LISTENNER)
		{
			int domain = listenner.is_unix ? AF_UNIX : (listenner.is_ipv6 ? AF_INET6 : AF_INET);
			fd(socket(domain, SOCK_STREAM, 0));
			if (fd() < 0)
			{
				_errors.push_back(std::string("Error on create listenner socket as SOCK_STREAM: ") + strerror(errno));
				printErrors();
				return;
			}
			_setSocketOptions(listenner);
			_setAddr(listenner);
			if (hasErrors())
			{
				std::cerr << "Has socket listenner errors. Cannot bind & listen:\n";
				printErrors();
				std::cerr << std::endl;
				return;
			}
			_bind_and_listen(worker_connections);
			std::cout << "Server listenning on " << _addr.ip() << ":" << _addr.port() << "..." << std::endl;
		};
		~SocketListenner() {};
};

#endif
