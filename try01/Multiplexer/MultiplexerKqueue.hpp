/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultiplexerKqueue.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 22:00:25 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/11 13:46:15 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MULTIPLEXER_KQUEUE_HPP
# define MULTIPLEXER_KQUEUE_HPP

/*
	GENTEEEE!

	O multiplexer Kqueue é permitido pelo subject, mas ele é exclusivo para a fampilia BSD (como macOS, FreeBSD, OpenBSD e NetBSD)
	Entao só vou deixar aqui :(

*/

# if defined(__APPLE__) && defined(__MACH__)

# include <sys/types.h>
# include <sys/event.h>

# include "IMultiplexer.hpp"
# include "Socket.hpp"
# include "SocketEvent.hpp"

class MultiplexerKqueue
{
	private:


	public:
		MultiplexerKqueue() {};
		~MultiplexerKqueue() {};

		void		add(Socket *socket)
		{

		}

		void		remove(Socket *socket)
		{

		}

		std::string	wait(SocketEventList &events)
		{
			return "";
		}
};

# endif

#endif
