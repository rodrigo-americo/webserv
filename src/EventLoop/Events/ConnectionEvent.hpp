/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionEvent.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 16:16:18 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/07/05 16:43:24 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_EVENT_HPP
# define SOCKET_EVENT_HPP

# include <vector>
# include <string>
# include <iostream>

class FileDescriptor;

struct ConnectionEvent
{
	FileDescriptor	*file_descriptor;
	bool			readable;
	bool			writable;
	bool			eof;
	std::string		error;

	ConnectionEvent(): file_descriptor(NULL), readable(false), writable(false), eof(false), error() {}
};

typedef std::vector<ConnectionEvent>	ConnectionEventList;

std::ostream	&operator<<(std::ostream &os, const ConnectionEvent &ev);


#endif
