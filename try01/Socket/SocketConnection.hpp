/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketConnection.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 01:17:51 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/29 18:50:50 by ighannam         ###   ########.fr       */
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
			fd(accept(_listenner->fd(), _addr.ptr(), &len));
			if (fd() < 0)
			{
				_errors.push_back(std::string("accept fail: ") + strerror(errno));
				printErrors();
				return;
			}
			_addr.recalculate();
			// O_CLOEXEC -> Isso evita que o `fd` vaze para processos filhos do `CGI`.
			// Sem isso, quando você der `fork() + execve()` para rodar um `CGI`, o processo filho vai herdar o `fd` da conexão do cliente — o que é um problema de segurança e pode causar comportamento estranho. O subject explicitamente permite `FD_CLOEXEC`.
			// fcntl(fd(), F_SETFL, O_NONBLOCK | O_CLOEXEC); -> verificar se o correto é assim ou como está abaixo
			// F_SETFL (file status flags) trabalha com flags prefixadas O_* — controla o comportamento das operações de I/O. O_NONBLOCK é uma dessas.
			// F_SETFD (file descriptor flags) trabalha com flags prefixadas FD_* — controla o comportamento do fd como handle. FD_CLOEXEC é a única padronizada.
			fcntl(fd(), F_SETFL, O_NONBLOCK);
			fcntl(fd(), F_SETFD, FD_CLOEXEC);
			std::cout << "client connected! fd: " << fd() << std::endl;
		}
		SocketConnection(const SocketConnection& other): Socket(other), _listenner(other._listenner) { LOG_TRACE("copy contructor SocketConnection called " << other.fd() << "\n"); };
		~SocketConnection() {}

		const Socket	*listenner() const { return _listenner; }
};

#endif
