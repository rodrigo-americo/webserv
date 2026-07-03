/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 11:21:30 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/29 18:58:56 by ighannam         ###   ########.fr       */
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
# include "Logger.hpp"



struct SocketType
{
	enum type
	{
		LISTENNER,
		CONNECTION,
		PIPE_READ,
		PIPE_WRITE
	};
};

class Socket: public segregation::has_type<SocketType::type>
{
	typedef segregation::has_type<SocketType::type>	base;
	public:
		typedef SocketType::type	type;
	private:
		int							_fd;
	protected:
		SocketAddress				_addr;
		std::vector<std::string>	_errors;

	void	_close() 
	{ 
		if (_fd > 2)
		{
			LOG_TRACE("is closing fd: " << _fd << "\n");
			::close(_fd);
		} 
	}

	public:
		Socket(type _type): base(_type), _fd(-1), _addr(), _errors() {};
		Socket(type _type, int fd): base(_type), _fd(fd), _addr(), _errors() {};
		Socket(const Socket& other): base(other._type), _fd(other._fd), _addr(other._addr), _errors(other._errors) {LOG_TRACE("copy contructor Socket called " << other._fd << "\n");};
		~Socket() { _close(); };

		ssize_t	read(size_t bytes, std::string &buff) const
		{
			char buffer[bytes];
			ssize_t bytes_read = ::read(_fd, buffer, bytes);
			if (bytes_read > 0)
				buff += std::string(buffer, bytes_read);
			return bytes_read;
		}

		void	write(const std::string &data) const
		{
			::write(_fd, data.c_str(), data.size());
		}

		int		fd() const { return _fd; }

		void fd(int fd_)
		{
			int temp = fd();
			_fd = fd_;
			LOG_TRACE("fd conn set fd " << temp << " to " << fd_ << "\n");
		}

		const SocketAddress	&addr() const { return _addr; }
		SocketAddress	&addr() { return _addr; }

		bool							isValid() const { return _fd > 2; }
		bool							hasErrors() const { return !_errors.empty(); }
		const std::vector<std::string>	&errors() const { return _errors; }
		void							printErrors() const
		{
			for (size_t i = 0; i < _errors.size(); i++)
				std::cerr << _errors[i] << "\n";
		}
		static bool usesGetSockOpt(SocketType::type t)
		{
			return t == SocketType::LISTENNER || t == SocketType::CONNECTION;
		}
};

#endif
