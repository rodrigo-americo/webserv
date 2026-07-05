/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketPipeWrite.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/28 18:47:02 by ighannam          #+#    #+#             */
/*   Updated: 2026/07/05 13:38:11 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETPIPEWRITE_HPP
#define SOCKETPIPEWRITE_HPP

# include <fcntl.h>
# include <errno.h>
# include <cstring>

# include "PipeChannel.hpp"
// #include "Socket.hpp"

class SocketPipeWrite : public PipeChannel
{
public:
    SocketPipeWrite(int _fd): PipeChannel(SocketType::PIPE_WRITE, _fd)
    {
        // LOG_TRACE("SocketPipeWrite contructor with fd: " << _fd << "\n");
        // int flags = fcntl(fd(), F_GETFL, 0);
        // if (flags < 0)
        //     _errors.push_back(std::string("fcntl F_GETFL fail: ") + strerror(errno));
        // else if (fcntl(fd(), F_SETFL, flags | O_NONBLOCK) < 0)
        //     _errors.push_back(std::string("fcntl F_SETFL O_NONBLOCK fail: ") + strerror(errno));
        // if (fcntl(fd(), F_SETFD, FD_CLOEXEC) < 0)
        //     _errors.push_back(std::string("fcntl F_SETFD fail: ") + strerror(errno));
    };
    ~SocketPipeWrite() {};
};



#endif
