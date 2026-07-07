/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IMultiplexer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 16:58:06 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/07/07 13:55:19 by bruno-valer      ###   ########.fr       */
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

# include "ConnectionEvent.hpp"

class FileDescriptor;

struct IMultiplexer
{
	virtual ~IMultiplexer() {};
	virtual void		add(FileDescriptor *file_descriptor) = 0;
	virtual void		remove(FileDescriptor *file_descriptor) = 0;
	virtual std::string	wait(ConnectionEventList &events) = 0;
	virtual void		setTimeout(int timeout_ms) = 0;
	virtual void flushRemovals() = 0;
};


#endif
