/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketPipeWrite.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/28 18:47:02 by ighannam          #+#    #+#             */
/*   Updated: 2026/06/29 19:04:51 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETPIPEWRITE_HPP
#define SOCKETPIPEWRITE_HPP

# include <fcntl.h>
# include <errno.h> 
# include <cstring>

#include "Socket.hpp"

class SocketPipeWrite : public Socket
{
public:
    SocketPipeWrite(int _fd): Socket(SocketType::PIPE_WRITE, _fd)
    {
        LOG_TRACE("SocketPipeWrite contructor with fd: " << _fd << "\n");
        int flags = fcntl(fd(), F_GETFL, 0);
        if (flags < 0)
            _errors.push_back(std::string("fcntl F_GETFL fail: ") + strerror(errno));
        else if (fcntl(fd(), F_SETFL, flags | O_NONBLOCK) < 0)
            _errors.push_back(std::string("fcntl F_SETFL O_NONBLOCK fail: ") + strerror(errno));
        if (fcntl(fd(), F_SETFD, FD_CLOEXEC) < 0)
            _errors.push_back(std::string("fcntl F_SETFD fail: ") + strerror(errno));
    };
    ~SocketPipeWrite() {};
};



#endif