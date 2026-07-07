#ifndef PIPE_HPP
# define PIPE_HPP

# include <unistd.h>


# include "PipeChannel.hpp"

class Pipe
{
private:

	int	_close(PipeChannel *channel)
	{
		if (!channel) return 0;
		int status = channel->close();
		delete channel;
		return status;
	}

public:
	bool		error;
	PipeChannel	*read;
	PipeChannel	*write;

	Pipe(): error(false), read(NULL), write(NULL)
	{
		int fds[2];
		if (pipe(fds) == -1)
		{
			error = true;
			return;
		}
		read = new PipeChannel(FileDescriptorType::PIPE_READ, fds[0]);
		if (!read)
		{
			error = true;
			return;
		}
		write = new PipeChannel(FileDescriptorType::PIPE_WRITE, fds[1]);
		if (!write)
		{
			delete read;
			error = true;
		}
		if (read->hasErrors() || write->hasErrors())
		{
			delete read;
			delete write;
			error = true;
		}
	};
	~Pipe() {};

	int	closeRead()
	{
		return _close(read);
	}

	int	closeWrite()
	{
		return _close(write);
	}

	std::pair<int, int>	close()
	{
		std::pair<int, int> status;
		status.first = closeRead();
		status.second = closeWrite();
		return status;
	}
};

inline std::ostream	&operator<<(std::ostream &os, const Pipe &pipe)
{
	os << "Pipe(" << pipe.read->fd() << ", " << pipe.write->fd() << ")";
	return os;
}

#endif
