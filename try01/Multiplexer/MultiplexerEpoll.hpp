/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultiplexerEpoll.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 20:54:12 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/07/07 13:54:54 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MULTIPLEXER_EPOLL_HPP
# define MULTIPLEXER_EPOLL_HPP

# include <unistd.h>
# include <errno.h>
# include <sys/epoll.h>
# include <vector>

# include "IMultiplexer.hpp"
# include "Socket.hpp"
# include "ConnectionEvent.hpp"

class MultiplexerEpoll: public IMultiplexer
{
	private:
		typedef std::vector<FileDescriptor *>	file_descriptors;
		int	_epollfd;
		int	_timeout_ms;
		file_descriptors _pending_deletion;

	public:
		// sobre `epoll_create` (info retirada do `man epoll_create`) -> Since Linux 2.6.8, the size argument is ignored, but must be greater than zero;
		MultiplexerEpoll(): _epollfd(epoll_create(1)), _timeout_ms(-1) {};
		void setTimeout(int timeout_ms) { _timeout_ms = timeout_ms; }
		~MultiplexerEpoll()
		{
			if (_epollfd > 2)
				close(_epollfd);
		};

		void		add(FileDescriptor *file_descriptor)
		{
			if (!file_descriptor) return;

			epoll_event	event;

			event.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP;
			event.data.ptr = file_descriptor;
			epoll_ctl(_epollfd, EPOLL_CTL_ADD, file_descriptor->fd(), &event);
		}

		void		remove(FileDescriptor *file_descriptor)
		{
			if (!file_descriptor) return;
			epoll_ctl(_epollfd, EPOLL_CTL_DEL, file_descriptor->fd(), NULL);
			_pending_deletion.push_back(file_descriptor);
		}

		void flushRemovals()
        {
            for (file_descriptors::iterator it = _pending_deletion.begin(); it != _pending_deletion.end(); ++it)
                delete *it;
            _pending_deletion.clear();
        }

		std::string	wait(ConnectionEventList &events)
		{
			epoll_event	epoll_events[1024];

			int n = epoll_wait(_epollfd, epoll_events, 1024, _timeout_ms);
			if (n < 0)
				return strerror(errno);
			for (int i = 0; i < n; i++)
			{
				ConnectionEvent	event;

				event.file_descriptor = static_cast<Socket*>(epoll_events[i].data.ptr);
				event.readable	= epoll_events[i].events & EPOLLIN;
				event.writable	= epoll_events[i].events & EPOLLOUT;
				if (epoll_events[i].events & EPOLLERR)
				{
					if (Socket::usesGetSockOpt(event.file_descriptor->getType()))
					{
						int err;
						socklen_t len = sizeof(err);
						if (getsockopt(event.file_descriptor->fd(), SOL_SOCKET, SO_ERROR, &err, &len) < 0)
							return strerror(errno);
						if (err != 0)
							event.error = "epoll error: " + std::string(strerror(err));
					}
					else
					{
						event.error = "pipe error";
					}
				}
				else if (epoll_events[i].events & EPOLLHUP)
				{
					// event.error = "epoll error: client has disconnected!";
					event.eof = true;
				}
				events.push_back(event);
			}
			return "";
		}
};

#endif
