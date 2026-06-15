/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestBuilder.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 02:31:28 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/11 17:35:24 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQUEST_BUILDER_HPP
# define HTTP_REQUEST_BUILDER_HPP

# include <string>

# include "HttpRequest.hpp"
# include "HttpHeaders.hpp"
# include "SocketConnection.hpp"

// ISSO E UMA CLASSE MERDA DEPOIS TEM QUE MELHORAR
class RequestBuilder
{
private:
	HttpRequest											_req;
	std::string										_buffer;
	size_t											_body_start;
	size_t											_is_request_line_processed;
	bool											_is_complete;
	size_t											_cursor;

	void	_processHeader(size_t last)
	{
		if (!_is_request_line_processed)
		{
			_cursor = _buffer.find_first_of(' ');
			std::string	line_method = _buffer.substr(0, _cursor); // method
			if (line_method == "GET")
				_req.method = RequestMethod::GET;
			else if (line_method == "POST")
				_req.method = RequestMethod::POST;
			else if (line_method == "PUT")
				_req.method = RequestMethod::PUT;
			else if (line_method == "PATCH")
				_req.method = RequestMethod::PATCH;
			else if (line_method == "DELETE")
				_req.method = RequestMethod::DELETE;
			_cursor = _buffer.find_first_not_of(' ', _cursor); // path
			_req.path = _buffer.substr(_cursor, _buffer.find_first_of(' ', _cursor) - _cursor);
			_cursor = _buffer.find_first_not_of(' ', _cursor); // http_version
			_req.http_version = _buffer.substr(_cursor, _buffer.find_first_of('\r') - _cursor);
			_cursor += 2;
		}
		_is_request_line_processed = true;
		while (_cursor <= last)
		{
			size_t	begin = _cursor;
			_cursor = _buffer.find_first_of(':', _cursor);
			std::string	key = _buffer.substr(begin, _cursor - begin);
			begin = _buffer.find_first_not_of(' ', _cursor);
			_cursor = _buffer.find_first_of('\r', _cursor);
			std::string	value = _buffer.substr(begin, _cursor - begin);
			_req.headers[key] = value;
		}
		_verifyCompletion();
	}

	void	_verifyCompletion()
	{
		if (_body_start == 0) return;

		if ( _req.method != RequestMethod::GET)
		{
			const HttpRequest &req = _req;
			int	body_size = _buffer.size() - _body_start;
			_is_complete = body_size >= std::atoi(req.headers.content_length().c_str());
			_req.body = _buffer.substr(_body_start, body_size);
		}
		_is_complete = true;
}

public:
	SocketConnection *connection;
	RequestBuilder(SocketConnection *conn)
		: _req(conn), _buffer(), _body_start(0), _is_request_line_processed(false), _is_complete(false), _cursor(0), connection(conn) {}
	~RequestBuilder() {}

	HttpRequest	build() const { return _req; }

	/* complete e true nesse metodos quando a quantidade lida e menor do que o `chunck-size` */
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
			_processHeader(idx - 1);
		}
	}

	bool	isComplete() const { return _is_complete; }
};

#endif
