#include <unistd.h>
#include <fcntl.h>

#include "FileChunkSource.hpp"

const size_t FileChunkSource::CHUNK_SIZE;

FileChunkSource::FileChunkSource(): _fd(-1), _buffer(), _buffer_off(0), _left(0) {}

FileChunkSource::~FileChunkSource() { close(); }

bool	FileChunkSource::open(const std::string &path, size_t size)
{
	_fd = ::open(path.c_str(), O_RDONLY);
	if (_fd < 0)
		return false;
	_buffer.clear();
	_buffer_off = 0;
	_left = size;
	return true;
}

void	FileChunkSource::close()
{
	if (_fd >= 0)
	{
		::close(_fd);
		_fd = -1;
	}
}

bool	FileChunkSource::isOpen() const { return _fd >= 0; }

bool	FileChunkSource::refill()
{
	char	chunk[CHUNK_SIZE];
	size_t	to_read = _left < CHUNK_SIZE ? _left : CHUNK_SIZE;
	ssize_t	n = ::read(_fd, chunk, to_read);
	if (n <= 0)
		return false;
	_buffer.assign(chunk, n);
	_buffer_off = 0;
	return true;
}

bool	FileChunkSource::needsRefill() const { return _buffer_off >= _buffer.size(); }

const char	*FileChunkSource::data() const { return _buffer.data() + _buffer_off; }
size_t		FileChunkSource::pending() const { return _buffer.size() - _buffer_off; }

void	FileChunkSource::advance(size_t n) { _buffer_off += n; _left -= n; }

bool	FileChunkSource::done() const { return _left == 0 && needsRefill(); }
