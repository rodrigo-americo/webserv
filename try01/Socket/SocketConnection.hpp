/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketConnection.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 01:17:51 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/11 17:35:46 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_CONNECTION_HPP
# define SOCKET_CONNECTION_HPP

# include <errno.h>
# include <fcntl.h>

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
			// O_CLOEXEC -> Isso evita que o `fd` vaze para processos filhos do `CGI`.
			// Sem isso, quando você der `fork() + execve()` para rodar um `CGI`, o processo filho vai herdar o `fd` da conexão do cliente — o que é um problema de segurança e pode causar comportamento estranho. O subject explicitamente permite `FD_CLOEXEC`.
			fcntl(_fd, F_SETFL, O_NONBLOCK | O_CLOEXEC);
			std::cout << "client connected! fd: " << _fd << std::endl;
		}
		~SocketConnection() {}

		const Socket	*listenner() const { return _listenner; }
};

#endif
