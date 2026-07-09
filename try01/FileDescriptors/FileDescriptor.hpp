#ifndef FILE_DECRIPTOR_HPP
# define FILE_DECRIPTOR_HPP

# include <unistd.h>
# include <vector>

# include "segregation.hpp"
# include "Logger.hpp"

struct FileDescriptorType
{
	enum type
	{
		SOCKET_LISTENNER,
		SOCKET_CONNECTION,
		PIPE_READ,
		PIPE_WRITE
	};
};

class FileDescriptor: public segregation::has_type<FileDescriptorType::type>
{
	typedef segregation::has_type<FileDescriptorType::type>	has_type;
	public:
		typedef FileDescriptorType::type	type;
	private:
		int	_fd;

	protected:
		std::vector<std::string>	_errors;

	public:
		FileDescriptor(type _type): has_type(_type), _fd(-1) {}
		FileDescriptor(type _type, int fd): has_type(_type), _fd(fd) {}
		FileDescriptor(const FileDescriptor& other): has_type(other._type), _fd(other._fd) {LOG_TRACE("copy contructor Socket called " << other._fd << "\n");};
		~FileDescriptor() { close(); }

		ssize_t	read(size_t bytes, std::string &buff) const
		{
			char buffer[bytes];
			ssize_t bytes_read = ::read(_fd, buffer, bytes);
			if (bytes_read > 0)
				buff += std::string(buffer, bytes_read);
			return bytes_read;
		}

		int	close()
		{
			if (_fd > 2)
			{
				LOG_TRACE("is closing fd: " << _fd);
				int status = ::close(_fd);
				_fd = -1;
				return status;
			}
			return 0;
		}

		void	write(const std::string &data) const
		{
			::write(_fd, data.c_str(), data.size());
		}

		int dup2(int _fd)
		{
			int status = ::dup2(fd(), _fd);
			if (status != -1)
				close();
			return status;
		}

		int		fd() const { return _fd; }

		void fd(int fd_)
		{
			int temp = fd();
			_fd = fd_;
			LOG_TRACE("fd conn set fd " << temp << " to " << fd_ << "\n");
		}

		bool							isClosed() const { return _fd == -1; }
		bool							isValid() const { return _fd > 2; }
		bool							hasErrors() const { return !_errors.empty(); }
		const std::vector<std::string>	&errors() const { return _errors; }
		void							printErrors() const
		{
			for (size_t i = 0; i < _errors.size(); i++)
				std::cerr << _errors[i] << "\n";
		}
};

#endif
