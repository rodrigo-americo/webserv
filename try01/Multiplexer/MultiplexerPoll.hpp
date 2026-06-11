/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultiplexerPoll.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 17:06:31 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/10 14:33:40 by bruno-valer      ###   ########.fr       */
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

	public:
		MultiplexerPoll(): _sockets(), _pollfds() {};
		~MultiplexerPoll() {};

		void add(Socket *socket)
		{
			if (!socket) return;
			if (std::binary_search(_sockets.begin(), _sockets.end(), socket))
				return;

			pollfd	poll_fd;

			poll_fd.fd		= socket->fd();
			poll_fd.events	= POLLIN;
			poll_fd.revents	= 0;

			sockets::iterator	it = std::lower_bound(_sockets.begin(), _sockets.end(), socket);
			size_t	idx = it - _sockets.begin();
			_sockets.insert(it, socket);
			_pollfds.insert(_pollfds.begin() + idx, poll_fd);
		}

		void remove(Socket *socket)
		{
			if (!socket) return;

			sockets::iterator	it = std::lower_bound(_sockets.begin(), _sockets.end(), socket);
			if (it == _sockets.end()) return;
			size_t	idx = it - _sockets.begin();
			delete *it;
			_sockets.erase(it);
			_pollfds.erase(_pollfds.begin() + idx);
		}

		std::string wait(SocketEventList &events)
		{
			events.clear();
			int ret = poll(&_pollfds[0], _pollfds.size(), -1);
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
					if (getsockopt(_sockets[i]->fd(), SOL_SOCKET, SO_ERROR, &err, &len) < 0)
					{
						events.clear();
						return strerror(errno);
					}
					if (err != 0)
						event.error = "poll error: " + std::string(strerror(err));
				}
				else if (_pollfds[i].revents & POLLHUP)
					event.error = "poll error: client has disconnected!";
				else if (_pollfds[i].revents & POLLNVAL)
					event.error = "poll error: invalid fd '" + utils::to_string(_sockets[i]->fd()) + "'!";
				events.push_back(event);
			}
			return "";
		}
};

#endif
