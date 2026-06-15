/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketAddress.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 15:21:20 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/10 01:24:47 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKER_ADDRESS_HPP
# define SOCKER_ADDRESS_HPP

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

		void	toIpv4(unsigned short port, in_addr_t s_addr = INADDR_ANY)
		{
			_storage.ss_family = AF_INET;
			sockaddr_in	*ipv4 = reinterpret_cast<sockaddr_in *>(&_storage);
			ipv4->sin_port = htons(port);
			ipv4->sin_addr.s_addr = s_addr;
			recalculate();
		}

		void	toIpv6(unsigned short port, in6_addr addr = ::in6addr_any)
		{
			_storage.ss_family = AF_INET6;
			sockaddr_in6	*ipv6 = reinterpret_cast<sockaddr_in6 *>(&_storage);
			ipv6->sin6_port = htons(port);
			ipv6->sin6_addr = addr;
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
