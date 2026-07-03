/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketPipeRead.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/28 18:46:27 by ighannam          #+#    #+#             */
/*   Updated: 2026/06/30 20:08:50 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETPIPEREAD_HPP
#define SOCKETPIPEREAD_HPP

# include <fcntl.h>
# include <errno.h> 
# include <cstring>

#include "Socket.hpp"


class SocketPipeRead : public Socket
{
public:
    SocketPipeRead(int _fd): Socket(SocketType::PIPE_READ, _fd)
    {
        int flags = fcntl(fd(), F_GETFL, 0);
        if (flags < 0)
            _errors.push_back(std::string("fcntl F_GETFL fail: ") + strerror(errno));
        else if (fcntl(fd(), F_SETFL, flags | O_NONBLOCK) < 0)
            _errors.push_back(std::string("fcntl F_SETFL O_NONBLOCK fail: ") + strerror(errno));
        if (fcntl(fd(), F_SETFD, FD_CLOEXEC) < 0)
            _errors.push_back(std::string("fcntl F_SETFD fail: ") + strerror(errno));
    };
    ~SocketPipeRead() {};
};




#endif