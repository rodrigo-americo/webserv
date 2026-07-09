#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <unistd.h>

# include <iostream>
# include <vector>

# include "segregation.hpp"
# include "ConfigServerListen.hpp"
# include "SocketAddress.hpp"
# include "Logger.hpp"
# include "FileDescriptor.hpp"

class Socket: public FileDescriptor
{
	public:
		typedef FileDescriptorType::type	type;
	protected:
		SocketAddress				_addr;

	public:
		Socket(type _type): FileDescriptor(_type), _addr() {};
		Socket(type _type, int fd): FileDescriptor(_type, fd), _addr() {};
		Socket(const FileDescriptor &fd): FileDescriptor(fd), _addr() {};
		~Socket() {};

		const SocketAddress	&addr() const { return _addr; }
		SocketAddress		&addr() { return _addr; }
		static bool usesGetSockOpt(FileDescriptorType::type t)
		{
			return t == FileDescriptorType::SOCKET_LISTENNER || t == FileDescriptorType::SOCKET_CONNECTION;
		}
};

#endif
