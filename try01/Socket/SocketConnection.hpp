/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketConnection.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 01:17:51 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/07/04 00:35:24 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_CONNECTION_HPP
# define SOCKET_CONNECTION_HPP

# include <errno.h>
# include <fcntl.h>
# include <unistd.h>
# include <sys/sendfile.h>

# include "str.hpp"
# include "Socket.hpp"

class SocketConnection: public Socket
{
	private:
		const Socket *_listenner;
		std::string	_out_headers;
		size_t		_out_headers_off;
		int			_out_file_fd;
		off_t		_out_file_off;
		size_t		_out_file_left;

	public:
		SocketConnection(const Socket *listenner): Socket(SocketType::CONNECTION), _listenner(listenner),
			_out_headers(), _out_headers_off(0), _out_file_fd(-1), _out_file_off(0), _out_file_left(0)
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
		SocketConnection(const SocketConnection& other): Socket(other), _listenner(other._listenner),
			_out_headers(other._out_headers), _out_headers_off(other._out_headers_off),
			_out_file_fd(-1), _out_file_off(0), _out_file_left(0)
		{ LOG_TRACE("copy contructor SocketConnection called " << other.fd() << "\n"); };

		~SocketConnection()
		{
			if (_out_file_fd >= 0)
				close(_out_file_fd);
		}

		const Socket	*listenner() const { return _listenner; }

		bool	hasPendingWrite() const
		{
			return _out_headers_off < _out_headers.size() || _out_file_fd >= 0;
		}

		void	queueWrite(const utils::str &data)
		{
			_out_headers = data.string();
			_out_headers_off = 0;
		}

		bool	queueFile(const utils::str &path, size_t size)
		{
			_out_file_fd = open(path.c_str(), O_RDONLY);
			if (_out_file_fd < 0)
				return false;
			_out_file_off = 0;
			_out_file_left = size;
			return true;
		}

		void	flushWrite()
		{
			if (_out_headers_off < _out_headers.size())
			{
				ssize_t n = ::write(fd(), _out_headers.data() + _out_headers_off, _out_headers.size() - _out_headers_off);
				if (n > 0)
					_out_headers_off += static_cast<size_t>(n);
				return;
			}
			if (_out_file_fd < 0)
				return;
			ssize_t n = sendfile(fd(), _out_file_fd, &_out_file_off, _out_file_left);
			if (n <= 0)
			{
				close(_out_file_fd);
				_out_file_fd = -1;
				return;
			}
			_out_file_left -= static_cast<size_t>(n);
			if (_out_file_left == 0)
			{
				close(_out_file_fd);
				_out_file_fd = -1;
			}
		}
};

#endif
