/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketAddress.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 15:21:20 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/07/09 20:44:45 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKER_ADDRESS_HPP
# define SOCKER_ADDRESS_HPP

# include <sys/socket.h>
# include <netdb.h>
# include <netinet/in.h>
# include <sys/un.h>
# include <string>

class SocketAddress
{
	private:
		sockaddr_storage	_storage;
		socklen_t			_length;
		unsigned short		_port;
		sockaddr			*_ptr;

	public:
		SocketAddress(sockaddr_storage &storage)
			: _storage(storage), _length(sizeof(sockaddr_storage)), _port(0), _ptr(reinterpret_cast<sockaddr *>(&_storage))
				{ recalculate(); }
		SocketAddress()
			: _storage(), _length(sizeof(sockaddr_storage)), _port(0), _ptr(reinterpret_cast<sockaddr *>(&_storage))
				{}
		~SocketAddress() {};

		struct addrinfo *resolveAddrInfo(const std::string &ip, int family)
		{

			std::string _ip = ip.empty() ? "127.0.0.1" : ip;
			struct addrinfo hints;
			struct addrinfo *result;

			std::memset(&hints, 0, sizeof(hints));
			hints.ai_family = family;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_flags = AI_NUMERICHOST;

			int status = getaddrinfo(_ip.c_str(), NULL, &hints, &result);
			if (status != 0)
				throw std::runtime_error(std::string("IP inválido: ") + gai_strerror(status));

			return result; // quem chamou é responsável por dar freeaddrinfo()
		}

		void	toIpv4(unsigned short port, const std::string &ip)
		{
			_storage.ss_family = AF_INET;
			sockaddr_in	*ipv4 = reinterpret_cast<sockaddr_in *>(&_storage);
			ipv4->sin_port = htons(port);

			struct addrinfo *result = resolveAddrInfo(ip, _storage.ss_family);
			struct sockaddr_in *addr_in = reinterpret_cast<sockaddr_in *>(result->ai_addr);
			ipv4->sin_addr.s_addr = addr_in->sin_addr.s_addr;
			freeaddrinfo(result);
			recalculate();
		}

		void	toIpv6(unsigned short port, const std::string &ip)
		{
			_storage.ss_family = AF_INET6;
			sockaddr_in6	*ipv6 = reinterpret_cast<sockaddr_in6 *>(&_storage);
			ipv6->sin6_port = htons(port);

			struct addrinfo *result = resolveAddrInfo(ip, _storage.ss_family);
			struct sockaddr_in6 *addr_in = reinterpret_cast<sockaddr_in6 *>(result->ai_addr);
			ipv6->sin6_addr = addr_in->sin6_addr;
			freeaddrinfo(result);
			recalculate();
		}

		void	toUnix(const std::string &path)
		{
			_storage.ss_family = AF_UNIX;
			sockaddr_un	*unix_addr = reinterpret_cast<sockaddr_un *>(&_storage);
			::strcpy(unix_addr->sun_path, path.c_str());
			recalculate();
		}

		void	recalculate()
		{
			if (isIpv4())
			{
				_port = ntohs(reinterpret_cast<const sockaddr_in*>(&_storage)->sin_port);
				_length = sizeof(sockaddr_in);
			}
			else if (isIpv6())
			{
				_port = ntohs(reinterpret_cast<const sockaddr_in6*>(&_storage)->sin6_port);
				_length = sizeof(sockaddr_in6);
			}
			else if (isUnix())
				_length = sizeof(sockaddr_un);
		}

		bool			isIpv4() const { return _storage.ss_family == AF_INET; }
		bool			isIpv6() const { return _storage.ss_family == AF_INET6; }
		bool			isUnix() const { return _storage.ss_family == AF_UNIX; }

		unsigned short	port() const { return _port; }

		sockaddr	*ptr() { return _ptr; }
		const sockaddr	*ptr() const { return _ptr; }

		socklen_t	size() const { return _length; }
};

#endif
