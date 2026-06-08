/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionSocket.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 23:51:04 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/08 00:36:31 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_SOCKET_HPP
# define CONNECTION_SOCKET_HPP

# include <netinet/in.h>

# include "Socket.hpp"

class ConnectionSocket: public Socket
{
	private:
		socklen_t	_client_len;
		int			_listenner_fd;

	public:
		ConnectionSocket(int listenner_fd)
			: Socket(SocketType::CONNECTION), _client_len(sizeof(struct sockaddr_in)), _listenner_fd(listenner_fd)
			{
				_fd.fd = accept(_listenner_fd, (sockaddr *)&_addr, &_client_len);
				_errors.push_back("Connection failed.");
			};
		~ConnectionSocket() {};

		socklen_t			len() const { _client_len; }
		int					listennerFd() const { _listenner_fd; }
};

#endif
