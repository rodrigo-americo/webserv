/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultiplexerSelect.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 17:44:46 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/09 21:17:48 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MULTIPLEXER_SELECT_HPP
# define MULTIPLEXER_SELECT_HPP

# include <errno.h>
# include <sys/select.h>
# include <map>

# include "IMultiplexer.hpp"
# include "Socket.hpp"

class MultiplexerSelect: public IMultiplexer
{
	private:
		typedef std::map<int, Socket*>	sockets;
		sockets	_sockets;
		fd_set	_main_read_set;
		fd_set	_main_write_set;
		fd_set	_main_error_set;
		int		_max_fd;

	public:
		MultiplexerSelect(): _sockets(), _main_read_set(), _main_write_set(), _main_error_set(), _max_fd(0) {};
		~MultiplexerSelect() {};

		void add(Socket *socket)
		{
			if (!socket) return;

			FD_SET(socket->fd(), &_main_read_set);
			FD_SET(socket->fd(), &_main_write_set);
			FD_SET(socket->fd(), &_main_error_set);
			_sockets[socket->fd()] = socket;
			if (socket->fd() > _max_fd)
				_max_fd = socket->fd();
		}

		void remove(Socket *socket)
		{
			if (!socket) return;

			sockets::iterator	it = _sockets.find(socket->fd());
			if (it == _sockets.end()) return;
			FD_CLR(it->first, &_main_read_set);
			FD_CLR(it->first, &_main_write_set);
			FD_CLR(it->first, &_main_error_set);
			delete it->second;
			_sockets.erase(it);
		}

		std::string wait(SocketEventList &events)
		{
			events.clear();

			/*
				É necessário fazer a cópia dos ..._set, porque o select vai destrui-los e manter
				apenas os fds que de fato sofreram os correspondentes eventos.
			*/

			fd_set	read_set = _main_read_set;
			fd_set	write_set = _main_write_set;
			fd_set	error_set = _main_error_set;

			// o primeiro parametro do select diz todos os fds que ele deve checkar, de zero ate o valor passado.
			int ret = select(_max_fd + 1, &read_set, &write_set, &error_set, NULL);
			if (ret < 0)
				return strerror(errno);
			for (sockets::iterator it = _sockets.begin(); it != _sockets.end(); ++it)
			{
				SocketEvent		event;
				int				fd = it->first;
				event.socket	= it->second;
				event.readable	= FD_ISSET(fd, &read_set);
				event.writable	= FD_ISSET(fd, &write_set);
				if (FD_ISSET(fd, &error_set))
				{
					int			err;
					socklen_t	len = sizeof(err);
					if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &len) < 0)
					{
						events.clear();
						return strerror(errno);
					}
					if (err != 0)
						event.error = "select error: " + std::string(strerror(err));
				}
				if (event.readable || event.writable || !event.error.empty())
					events.push_back(event);
			}
			return "";
		}
};

#endif
