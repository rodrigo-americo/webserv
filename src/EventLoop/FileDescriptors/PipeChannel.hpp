#ifndef PIPE_CHANNEL_HPP
#define PIPE_CHANNEL_HPP

# include <unistd.h>
# include <fcntl.h>
# include <errno.h>
# include <cstring>

#include "FileDescriptor.hpp"


class PipeChannel : public FileDescriptor
{
public:
	PipeChannel(FileDescriptorType::type type, int _fd): FileDescriptor(type, _fd)
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
};

inline std::ostream	&operator<<(std::ostream &os, const PipeChannel &channel)
{
	os << "PipeChannel(" << channel.fd() << ")";
	return os;
}


#endif
