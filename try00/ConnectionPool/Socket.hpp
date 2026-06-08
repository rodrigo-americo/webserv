/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 20:57:41 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/08 00:37:22 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <poll.h>
# include <unistd.h>
# include <netinet/in.h>
# include <cstring>

# include "segregation.hpp"

struct SocketType
{
	enum type
	{
		LISTENNER,
		CONNECTION,
	};
};


class Socket: public segregation::has_type<SocketType::type>
{
	public:
		typedef SocketType::type	type;
	protected:
		pollfd						_fd;
		sockaddr_in					_addr;
		std::vector<std::string>	_errors;

	public:
		Socket(type _type): segregation::has_type<type>(_type), _fd(), _addr(), _errors()
		{
			std::memset(&_addr, 0, sizeof(_addr));
		};
		~Socket() { if (_fd.fd > 2) close(_fd.fd); };

		operator int() const { return _fd.fd; }
		operator struct pollfd&() { return _fd; }
		operator struct pollfd*() { return &_fd; }

		Socket	&operator=(const pollfd &poll_fds)
		{
			_fd.fd = poll_fds.fd;
			_fd.events = poll_fds.events;
			_fd.revents = poll_fds.revents;
			return *this;
		}

		Socket	&operator=(const Socket &other)
		{
			*this = other._fd;
			_errors = other._errors;
			return *this;
		}

		int								fd() const { _fd.fd; }
		const sockaddr_in				&addr() const { _addr; }
		bool							hasErrors() const { !_errors.empty(); }
		const std::vector<std::string>	&errors() const { _errors; }
};

#endif
