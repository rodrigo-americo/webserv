/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketConnection.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 01:17:51 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/10 12:02:55 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_CONNECTION_HPP
# define SOCKET_CONNECTION_HPP

# include "Socket.hpp"

class SocketConnection: public Socket
{
	private:
		const Socket *_listenner;

	public:
		SocketConnection(const Socket *listenner): Socket(SocketType::CONNECTION), _listenner(listenner)
		{
			socklen_t	len = _addr.size();
			_fd = accept(_listenner->fd(), _addr.ptr(), &len);
			if (_fd < 0)
			{
				_errors.push_back(std::string("accept fail: ") + strerror(errno));
				printErrors();
				return;
			}
			_addr.recalculate();
			std::cout << "client connected! fd: " << _fd << std::endl;
		}
		~SocketConnection() {}

		const Socket	*listenner() const { return _listenner; }
};

#endif
