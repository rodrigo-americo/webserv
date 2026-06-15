/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IMultiplexer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 16:58:06 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/11 13:46:39 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IMULTIPLEXER_HPP
# define IMULTIPLEXER_HPP

# ifdef __linux__
#  define CURRENT_SO "linux"
# elif defined(__APPLE__) && defined(__MACH__)
#  define CURRENT_SO "mac"
# elif defined(_WIN32)
#  define CURRENT_SO "windows"
# else
#  define CURRENT_SO "unknown"
#endif

# include <string>

# include "SocketEvent.hpp"

class Socket;

struct IMultiplexer
{
	virtual ~IMultiplexer() {};
	virtual void		add(Socket *socket) = 0;
	virtual void		remove(Socket *socket) = 0;
	virtual std::string	wait(SocketEventList &events) = 0;
};


#endif
