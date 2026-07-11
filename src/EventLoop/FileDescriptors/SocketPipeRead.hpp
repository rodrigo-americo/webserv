/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketPipeRead.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/28 18:46:27 by ighannam          #+#    #+#             */
/*   Updated: 2026/07/07 13:36:03 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETPIPEREAD_HPP
#define SOCKETPIPEREAD_HPP

# include <fcntl.h>
# include <errno.h>
# include <cstring>

# include "PipeChannel.hpp"
// #include "Socket.hpp"


class SocketPipeRead : public PipeChannel
{
public:
    SocketPipeRead(int _fd): PipeChannel(FileDescriptorType::PIPE_READ, _fd)
    {
        // int flags = fcntl(fd(), F_GETFL, 0);
        // if (flags < 0)
        //     _errors.push_back(std::string("fcntl F_GETFL fail: ") + strerror(errno));
        // else if (fcntl(fd(), F_SETFL, flags | O_NONBLOCK) < 0)
        //     _errors.push_back(std::string("fcntl F_SETFL O_NONBLOCK fail: ") + strerror(errno));
        // if (fcntl(fd(), F_SETFD, FD_CLOEXEC) < 0)
        //     _errors.push_back(std::string("fcntl F_SETFD fail: ") + strerror(errno));
    };
    ~SocketPipeRead() {};
};




#endif
