/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultiplexerSelect.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 17:44:46 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/07/07 13:56:28 by bruno-valer      ###   ########.fr       */
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
		typedef std::map<int, FileDescriptor*>	file_descriptors;
		file_descriptors	_sockets;
		fd_set				_main_read_set;
		fd_set				_main_write_set;
		fd_set				_main_error_set;
		int					_max_fd;
		int					_timeout_ms;
		file_descriptors	_pending_deletion;

	public:
		MultiplexerSelect(): _sockets(), _main_read_set(), _main_write_set(), _main_error_set(), _max_fd(0), _timeout_ms(-1) {};
		void setTimeout(int timeout_ms) { _timeout_ms = timeout_ms; }
		~MultiplexerSelect() {};

		void add(FileDescriptor *file_descriptor)
		{
			if (!file_descriptor) return;

			FD_SET(file_descriptor->fd(), &_main_read_set);
			FD_SET(file_descriptor->fd(), &_main_write_set);
			FD_SET(file_descriptor->fd(), &_main_error_set);
			_sockets[file_descriptor->fd()] = file_descriptor;
			if (file_descriptor->fd() > _max_fd)
				_max_fd = file_descriptor->fd();
		}

		void remove(FileDescriptor *file_descriptor)
		{
			if (!file_descriptor) return;

			file_descriptors::iterator	it = _sockets.find(file_descriptor->fd());
			if (it == _sockets.end()) return;
			FD_CLR(it->first, &_main_read_set);
			FD_CLR(it->first, &_main_write_set);
			FD_CLR(it->first, &_main_error_set);
			_pending_deletion[it->first] = it->second;
			_sockets.erase(it);
		}

		void flushRemovals()
        {
            for (file_descriptors::iterator it = _pending_deletion.begin(); it != _pending_deletion.end(); ++it)
                delete it->second;
            _pending_deletion.clear();
        }

		std::string wait(ConnectionEventList &events)
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
			struct timeval	tv;
			struct timeval*	tvp = NULL;
			if (_timeout_ms >= 0)
			{
				tv.tv_sec  = _timeout_ms / 1000;
				tv.tv_usec = (_timeout_ms % 1000) * 1000;
				tvp = &tv;
			}
			int ret = select(_max_fd + 1, &read_set, &write_set, &error_set, tvp);
			if (ret < 0)
				return strerror(errno);
			for (file_descriptors::iterator it = _sockets.begin(); it != _sockets.end(); ++it)
			{
				ConnectionEvent		event;
				int				fd = it->first;
				event.file_descriptor	= it->second;
				event.readable	= FD_ISSET(fd, &read_set);
				event.writable	= FD_ISSET(fd, &write_set);
				if (FD_ISSET(fd, &error_set))
				{
					int			err;
					socklen_t	len = sizeof(err);
					if (Socket::usesGetSockOpt(event.file_descriptor->getType()))
					{
						if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &len) < 0)
						{
							events.clear();
							return strerror(errno);
						}
						if (err != 0)
							event.error = "select error: " + std::string(strerror(err));
					}
					else
					{
						event.error = "pipe error";
					}
				}
				if (event.readable || event.writable || !event.error.empty())
					events.push_back(event);
			}
			return "";
		}
};

#endif
