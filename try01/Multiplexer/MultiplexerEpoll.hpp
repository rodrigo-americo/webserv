/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultiplexerEpoll.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 20:54:12 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/11 16:02:06 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MULTIPLEXER_EPOLL_HPP
# define MULTIPLEXER_EPOLL_HPP

# include <unistd.h>
# include <errno.h>
# include <sys/epoll.h>

# include "IMultiplexer.hpp"
# include "Socket.hpp"
# include "SocketEvent.hpp"

class MultiplexerEpoll: public IMultiplexer
{
	private:
		int	_epollfd;

	public:
		// sobre `epoll_create` (info retirada do `man epoll_create`) -> Since Linux 2.6.8, the size argument is ignored, but must be greater than zero;
		MultiplexerEpoll(): _epollfd(epoll_create(1)) {};
		~MultiplexerEpoll()
		{
			if (_epollfd > 2)
				close(_epollfd);
		};

		void		add(Socket *socket)
		{
			if (!socket) return;

			epoll_event	event;

			event.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP;
			event.data.ptr = socket;
			epoll_ctl(_epollfd, EPOLL_CTL_ADD, socket->fd(), &event);
		}

		void		remove(Socket *socket)
		{
			if (!socket) return;
			epoll_ctl(_epollfd, EPOLL_CTL_DEL, socket->fd(), NULL);
			delete socket;
		}

		std::string	wait(SocketEventList &events)
		{
			epoll_event	epoll_events[1024];

			int n = epoll_wait(_epollfd, epoll_events, 1024, -1);
			if (n < 0)
				return strerror(errno);
			for (int i = 0; i < n; i++)
			{
				SocketEvent	event;

				event.socket = static_cast<Socket*>(epoll_events[i].data.ptr);
				event.readable	= epoll_events[i].events & EPOLLIN;
				event.writable	= epoll_events[i].events & EPOLLOUT;
				if (epoll_events[i].events & EPOLLERR)
				{
					int			err;
					socklen_t	len = sizeof(err);
					if (getsockopt(event.socket->fd(), SOL_SOCKET, SO_ERROR, &err, &len) < 0)
						return strerror(errno);
					if (err != 0)
						event.error = "epoll error: " + std::string(strerror(err));
				}
				else if (epoll_events[i].events & EPOLLHUP)
					event.error = "epoll error: client has disconnected!";
				events.push_back(event);
			}
			return "";
		}
};

#endif
