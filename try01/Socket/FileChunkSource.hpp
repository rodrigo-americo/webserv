#ifndef FILE_CHUNK_SOURCE_HPP
# define FILE_CHUNK_SOURCE_HPP

# include <string>

class FileChunkSource
{
	private:
		static const size_t CHUNK_SIZE = 64 * 1024;

		int			_fd;
		std::string	_buffer;
		size_t		_buffer_off;
		size_t		_left;

	public:
		FileChunkSource();
		~FileChunkSource();

		bool	open(const std::string &path, size_t size);
		void	close();
		bool	isOpen() const;

		bool	refill();
		bool	needsRefill() const;

		const char	*data() const;
		size_t		pending() const;

		void	advance(size_t n);
		bool	done() const;
};

#endif
