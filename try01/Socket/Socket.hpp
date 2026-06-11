/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 11:21:30 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/10 14:08:16 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <unistd.h>

# include <iostream>
# include <vector>

# include "segregation.hpp"
# include "ConfigServerListen.hpp"
# include "SocketAddress.hpp"



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
	typedef segregation::has_type<SocketType::type>	base;
	public:
		typedef SocketType::type	type;

	protected:
		int							_fd;
		SocketAddress				_addr;
		std::vector<std::string>	_errors;

	void	_close() { if (_fd > 2) ::close(_fd); }

	public:
		Socket(type _type): base(_type), _fd(-1), _addr(), _errors() {};
		Socket(type _type, int fd): base(_type), _fd(fd), _addr(), _errors() {};
		~Socket() { _close(); };

		void	read(size_t bytes, std::string &buff) const
		{
			char buffer[bytes];
			::bzero(buffer, bytes);
			::read(_fd, buffer, bytes);
			buff += buffer;
		}

		void	write(const std::string &data) const
		{
			::write(_fd, data.c_str(), data.size());
		}

		int		fd() const { return _fd; }

		const SocketAddress	&addr() const { return _addr; }
		SocketAddress	&addr() { return _addr; }

		bool							hasErrors() const { return !_errors.empty(); }
		const std::vector<std::string>	&errors() const { return _errors; }
		void							printErrors() const
		{
			for (size_t i = 0; i < _errors.size(); i++)
				std::cerr << _errors[i] << "\n";
		}
};

#endif
