#ifndef PIPE_CHANNEL_HPP
#define PIPE_CHANNEL_HPP

# include <unistd.h>
# include <fcntl.h>
# include <errno.h> 
# include <cstring>

#include "Socket.hpp"


class PipeChannel : public Socket
{
public:
    PipeChannel(SocketType::type type, int _fd): Socket(type, _fd)
    {
        int flags = fcntl(fd(), F_GETFL, 0);
        if (flags < 0)
            _errors.push_back(std::string("fcntl F_GETFL fail: ") + strerror(errno));
        else if (fcntl(fd(), F_SETFL, flags | O_NONBLOCK) < 0)
            _errors.push_back(std::string("fcntl F_SETFL O_NONBLOCK fail: ") + strerror(errno));
        if (fcntl(fd(), F_SETFD, FD_CLOEXEC) < 0)
            _errors.push_back(std::string("fcntl F_SETFD fail: ") + strerror(errno));
    };
    ~PipeChannel() {};

    int dup2(int _fd)
    {
        int status = ::dup2(fd(), _fd);
        if (status != -1)
            close();
        return status;
    }
};




#endif