/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketEvent.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 16:16:18 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/28 20:37:28 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_EVENT_HPP
# define SOCKET_EVENT_HPP

# include <vector>
# include <string>

class Socket;

struct SocketEvent
{
	Socket		*socket;
	bool		readable;
	bool		writable;
	bool        eof;
	std::string	error;

	SocketEvent(): socket(NULL), readable(false), writable(false), eof(false), error() {}
};

typedef std::vector<SocketEvent>	SocketEventList;


#endif
