/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketConnection.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunofer <brunofer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 01:17:51 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/07/10 20:27:44 by brunofer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_CONNECTION_HPP
# define SOCKET_CONNECTION_HPP

# include <errno.h>
# include <fcntl.h>
# include <unistd.h>
# include <ctime>

# include "WebServerConfig.hpp"
# include "FileChunkSource.hpp"
# include "str.hpp"
# include "Socket.hpp"

class SocketConnection: public Socket
{
	private:
		const Socket	*_listenner;
		std::string		_out_headers;
		size_t			_out_headers_off;
		FileChunkSource	_out_file;
		time_t			_timeout;

	public:
		SocketConnection(const Socket *listenner): Socket(FileDescriptorType::SOCKET_CONNECTION), _listenner(listenner),
			_out_headers(), _out_headers_off(0), _out_file(), _timeout(std::time(NULL))
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
    		_out_headers(other._out_headers), _out_headers_off(other._out_headers_off)
		{ LOG_TRACE("copy contructor SocketConnection called " << other.fd() << "\n"); };

		~SocketConnection(){
			_out_file.close();
		};

		const Socket	*listenner() const { return _listenner; }

		void	resetTimeout()
		{
			_timeout = std::time(NULL);
			std::cout << "resetting timout: " << _timeout << "\n";
		}

		bool	expired()
		{
			time_t	now = std::time(NULL);
			WebServerConfig	&config = WebServerConfig::getInstance();
			time_t timeout =  config.getGlobal()->getHttp()->getKeepaliveTimeout();
			timeout = timeout <= 0 ? 1000 : timeout * 1000;
			// std::cout << "now: " << now << " _timeout: " << _timeout << "\n";
			// std::cout << "expired diff: " << now - _timeout << " timeout: " << timeout << "\n";
			// std::cout << "expired: " << ((now - _timeout) > timeout ) << "\n";
			return ((now - _timeout) > timeout );
		}

		bool	hasPendingWrite() const
		{
			return _out_headers_off < _out_headers.size() || _out_file.isOpen();
		}

		void	queueWrite(const utils::str &data)
		{
			_out_headers = data.string();
			_out_headers_off = 0;
		}

		bool	queueFile(const utils::str &path, size_t size)
		{
			return _out_file.open(path.string(), size);
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
			if (!_out_file.isOpen())
				return;
			if (_out_file.needsRefill())
			{
				if (!_out_file.refill()){
					_out_file.close();
					return;
				}
			}
			ssize_t n = ::write(fd(), _out_file.data(), _out_file.pending());
			if ( n <= 0){
				_out_file.close();
				return;
			}
			_out_file.advance(static_cast<size_t>(n));
			if (_out_file.done())
				_out_file.close();
		}
};

#endif
