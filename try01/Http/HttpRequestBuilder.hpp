
#ifndef HTTP_REQUEST_BUILDER_HPP
# define HTTP_REQUEST_BUILDER_HPP

# include <string>
# include <stdlib.h>
# include <cerrno>
# include "HttpRequest.hpp"
# include "HttpHeaders.hpp"
# include "SocketConnection.hpp"
# include "WebServerConfig.hpp"
# include "ServerConfig.hpp"

class HttpRequestBuilder
{
private:
	HttpRequest	_req;
	std::string	_buffer;
	size_t		_body_start;
	size_t		_is_request_line_processed;
	bool		_is_complete;
	size_t		_cursor;

	enum ChunkState
	{
		CHUNK_SIZE,
		CHUNK_DATA,
		CHUNK_DATA_CRLF,
		CHUNK_TRAILER
	};

	static const size_t	MAX_CHUNK_SIZE = 100 * 1024 * 1024;
	static const size_t	MAX_TRAILER_SIZE = 8 * 1024;

	bool					_is_chunked;
	bool					_has_error;
	int						_error_status;
	std::string				_error_message;
	size_t					_chunk_cursor;
	size_t					_chunk_remaining;
	ChunkState				_chunk_state;
	size_t					_max_body_size;

	void	_processHeader(size_t header_end);
	void	_verifyCompletion();
	void	_decodeChunkedBody();
	bool	_decodeChunkSize();
	bool	_decodeChunkData();
	bool	_decodeChunkDataCrlf();
	bool	_decodeChunkTrailer();

public:
	SocketConnection *connection;

	HttpRequestBuilder(SocketConnection *conn)
		: _req(conn), _buffer(), _body_start(0), _is_request_line_processed(false), _is_complete(false), _cursor(0),
		  _is_chunked(false), _has_error(false), _error_status(400), _error_message("Bad Request"),
		  _chunk_cursor(0), _chunk_remaining(0), _chunk_state(CHUNK_SIZE), _max_body_size(0),
		  connection(conn) {}
	~HttpRequestBuilder() {}

	HttpRequest	build() const { return _req; }

	void	addToBuffer(const std::string &buff)
	{
		if (_is_complete) return;
		_buffer += buff;
		if (buff.size() > 0)
			_verifyCompletion();
		if (_body_start > 0) return;
		size_t	idx = _buffer.find("\r\n\r\n");
		if (idx != std::string::npos)
		{
			_body_start = idx + 4;
			_processHeader(idx);
		}
	}

	bool				isComplete() const { return _is_complete; }
	bool				hasError() const { return _has_error; }
	int					errorStatus() const { return _error_status; }
	const std::string	&errorMessage() const { return _error_message; }
	void				sendBadRequest() const;
};


#endif
