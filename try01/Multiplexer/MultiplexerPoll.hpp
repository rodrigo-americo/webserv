/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultiplexerPoll.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 17:06:31 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/07/05 16:53:29 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MULTIPLEXER_POLL_HPP
# define MULTIPLEXER_POLL_HPP

# include <errno.h>
# include <poll.h>
# include <vector>
# include <algorithm>

# include "IMultiplexer.hpp"
# include "Socket.hpp"

class MultiplexerPoll: public IMultiplexer
{
	private:
		typedef std::vector<Socket *>	sockets;
		typedef std::vector<pollfd>		pollfds;
		sockets	_sockets;
		pollfds	_pollfds;
		int		_timeout_ms;
		sockets _pending_deletion;

	public:
		MultiplexerPoll(): _sockets(), _pollfds(), _timeout_ms(-1) {};
		void setTimeout(int timeout_ms) { _timeout_ms = timeout_ms; }
		~MultiplexerPoll() {};

		void add(Socket *socket)
		{
			if (!socket) return;
			if (std::binary_search(_sockets.begin(), _sockets.end(), socket))
				return;

			pollfd	poll_fd;

			poll_fd.fd		= socket->fd();
			poll_fd.events	= POLLIN | POLLOUT | POLLERR | POLLHUP | POLLNVAL;
			poll_fd.revents	= 0;

			sockets::iterator	it = std::lower_bound(_sockets.begin(), _sockets.end(), socket);
			size_t	idx = it - _sockets.begin();
			_sockets.insert(it, socket);
			_pollfds.insert(_pollfds.begin() + idx, poll_fd);
		}

		void remove(Socket *socket)
		{
			if (!socket) return;
			LOG_TRACE("MULTIPLEXER remove fd: " << socket->fd());
			sockets::iterator	it = std::lower_bound(_sockets.begin(), _sockets.end(), socket);
			if (it == _sockets.end() || *it != socket)
				return;
			size_t	idx = it - _sockets.begin();
			_pending_deletion.push_back(*it);
			_sockets.erase(it);
			_pollfds.erase(_pollfds.begin() + idx);
		}

		void flushRemovals()
        {
            for (sockets::iterator it = _pending_deletion.begin(); it != _pending_deletion.end(); ++it)
                delete *it;
            _pending_deletion.clear();
        }

		std::string wait(SocketEventList &events)
		{
			events.clear();
			if (_pollfds.empty()) return "";
			int ret = poll(&_pollfds[0], _pollfds.size(), _timeout_ms);
			if (ret < 0) return strerror(errno);
			for (size_t i = 0; i < _sockets.size(); i++)
			{
				if (_pollfds[i].revents == 0)
					continue;
				SocketEvent		event;
				event.socket	= _sockets[i];
				event.readable	= _pollfds[i].revents & POLLIN;
				event.writable	= _pollfds[i].revents & POLLOUT;
				if (_pollfds[i].revents & POLLERR)
				{
					int			err;
					socklen_t	len = sizeof(err);
					if (Socket::usesGetSockOpt(_sockets[i]->getType()))
					{
						if (getsockopt(_sockets[i]->fd(), SOL_SOCKET, SO_ERROR, &err, &len) < 0)
						{
							events.clear();
							return strerror(errno);
						}
						if (err != 0)
							event.error = "poll error: " + std::string(strerror(err));
					}
					else
					{
						event.error = "pipe error";
					}
				}
				else if (_pollfds[i].revents & POLLHUP)
				{
					event.eof = true;
				}
				else if (_pollfds[i].revents & POLLNVAL)
					event.error = "poll error: invalid fd '" + utils::to_string(_sockets[i]->fd()) + "'!";
				events.push_back(event);
			}
			return "";
		}
};

#endif
