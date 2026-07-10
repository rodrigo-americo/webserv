#include "HttpRequestBuilder.hpp"
#include "HttpResponse.hpp"
#include "Router.hpp"

void HttpRequestBuilder::_processHeader(size_t header_end){
	if (!_is_request_line_processed){
		_cursor = _buffer.find_first_of(' ');
		if (_cursor == std::string::npos)
		{
			_has_error = true;
			return;
		}
		std::string line_method = _buffer.substr(0, _cursor);
		if (line_method == "GET")
			_req.method = RequestMethod::GET;
		else if (line_method == "POST")
			_req.method = RequestMethod::POST;
		else if (line_method == "DELETE")
			_req.method = RequestMethod::DELETE;
		else
			_req.method = RequestMethod::UNKNOWN;
		_cursor = _buffer.find_first_not_of(' ', _cursor);
		size_t line_end = _buffer.find_first_of('\r', _cursor);
		size_t path_end = _buffer.find_first_of(' ', _cursor);
		if (_cursor == std::string::npos || path_end == std::string::npos
			|| line_end == std::string::npos || path_end > line_end)
		{
			_has_error = true;
			return;
		}
		_req.path = Path(_buffer.substr(_cursor, path_end - _cursor));
		_cursor = _buffer.find_first_not_of(' ', path_end);
		if (_cursor == std::string::npos || _cursor > line_end)
		{
			_has_error = true;
			return;
		}
		size_t ver_end = _buffer.find_first_of('\r', _cursor);
		_req.http_version = _buffer.substr(_cursor, ver_end - _cursor);
		_cursor = ver_end + 2;
	}
	_is_request_line_processed = true;
	while (_cursor <= header_end)
	{
		size_t begin = _cursor;
		size_t colon = _buffer.find_first_of(':', _cursor);
		if (colon == std::string::npos || colon > header_end) break;
		std::string key = _buffer.substr(begin, colon - begin);
		size_t val_start = _buffer.find_first_not_of(' ', colon + 1);
		size_t val_end = _buffer.find_first_of('\r', colon + 1);
		if (val_end == std::string::npos || val_end > header_end) break;
		std::string	value = _buffer.substr(val_start, val_end - val_start);
		_req.headers[key] = value;
		_cursor = val_end + 2;
	}
	const ServerConfig *server = WebServerConfig::match_server(_req);
	if (server)
		_max_body_size = server->getClientMaxBodySize();
	if (_req.method != RequestMethod::GET){
		const std::string &te = _req.headers.transfer_encoding();
		if (!te.empty()){
			size_t f = te.find_first_not_of(" \t");
			size_t l = te.find_last_not_of(" \t");
			std::string trimmed = (f == std::string::npos) ? "" : te.substr(f, l - f + 1);
			if (trimmed == "chunked"){
				_is_chunked = true;
				_chunk_cursor = _body_start;
				_chunk_state = CHUNK_SIZE;
			}
			else
				_has_error = true;
		}
	}
	_verifyCompletion();
}

void HttpRequestBuilder::_verifyCompletion(){

	if (_body_start == 0 || _has_error) return;

	if (_is_chunked)
		_decodeChunkedBody();
	else if (_req.method != RequestMethod::GET){
		const std::string &cl = _req.headers.content_length();
		int expected = cl.empty() ? 0 : std::atoi(cl.c_str());
		if (_max_body_size > 0 && expected > 0 && static_cast<size_t>(expected) > _max_body_size){
			_has_error = true;
			_error_status = 413;
			_error_message = "Content too Large";
			return;
		}
		int body_size = static_cast<int>(_buffer.size() - _body_start);
		_req.body = _buffer.substr(_body_start, body_size);
		if (expected > 0)
			_is_complete = body_size >= expected;
		else
			_is_complete = true;
	}
	else
		_is_complete = true;
}

void	HttpRequestBuilder::_decodeChunkedBody()
{
	bool progressed = true;
	while (progressed && !_is_complete && !_has_error)
	{
		if (_chunk_state == CHUNK_SIZE)
			progressed = _decodeChunkSize();
		else if (_chunk_state == CHUNK_DATA)
			progressed = _decodeChunkData();
		else if (_chunk_state == CHUNK_DATA_CRLF)
			progressed = _decodeChunkDataCrlf();
		else
			progressed = _decodeChunkTrailer();
	}
}

bool	HttpRequestBuilder::_decodeChunkSize()
{
	// linha "hex[;ext]\r\n" — se não achar o \r\n ainda, espera mais bytes
	size_t line_end = _buffer.find("\r\n", _chunk_cursor);
	if (line_end == std::string::npos)
		return false;

	// chunk-extension (";foo=bar") é ignorada: corta a leitura do hex antes do ';'
	size_t size_end = _buffer.find_first_of(';', _chunk_cursor);
	if (size_end == std::string::npos || size_end > line_end)
		size_end = line_end;

	std::string size_str = _buffer.substr(_chunk_cursor, size_end - _chunk_cursor);
	while (!size_str.empty() && (size_str[size_str.size() - 1] == ' ' || size_str[size_str.size() - 1] == '\t'))
		size_str.erase(size_str.size() - 1);
	if (size_str.empty())
	{
		_has_error = true;
		return false;
	}

	char *endptr = NULL;
	errno = 0;
	unsigned long chunk_size = std::strtoul(size_str.c_str(), &endptr, 16);
	if (endptr == size_str.c_str() || *endptr != '\0' || errno == ERANGE)
	{
		_has_error = true;   // hex inválido, ex: "zz"
		return false;
	}
	if (chunk_size > MAX_CHUNK_SIZE)
	{
		_has_error = true;   // válido mas absurdo (>100MB num chunk só)
		return false;
	}

	_chunk_cursor = line_end + 2;
	if (chunk_size == 0)
		_chunk_state = CHUNK_TRAILER;
	else
	{
		_chunk_remaining = static_cast<size_t>(chunk_size);
		_chunk_state = CHUNK_DATA;
	}
	return true;
}

bool	HttpRequestBuilder::_decodeChunkData()
{
	size_t available = _buffer.size() - _chunk_cursor;
	if (available == 0)
		return false;   // nenhum dado do chunk chegou ainda

	size_t take = (available < _chunk_remaining) ? available : _chunk_remaining;

	if (_max_body_size > 0 && _req.body.size() + take > _max_body_size)
	{
		_has_error = true;
		_error_status = 413;
		_error_message = "Content Too Large";
		return false;
	}

	_req.body.append(_buffer, _chunk_cursor, take);   // acumula incremental, nunca reconstrói do zero
	_chunk_cursor += take;
	_chunk_remaining -= take;
	if (_chunk_remaining > 0)
		return false;   // dados do chunk cortados no meio, espera o resto

	_chunk_state = CHUNK_DATA_CRLF;
	return true;
}

bool	HttpRequestBuilder::_decodeChunkDataCrlf()
{
	if (_buffer.size() - _chunk_cursor < 2)
		return false;   // CRLF final do chunk ainda não chegou completo
	if (_buffer[_chunk_cursor] != '\r' || _buffer[_chunk_cursor + 1] != '\n')
	{
		_has_error = true;   // framing quebrado
		return false;
	}
	_chunk_cursor += 2;
	_chunk_state = CHUNK_SIZE;   // volta pro topo, próximo chunk
	return true;
}

bool	HttpRequestBuilder::_decodeChunkTrailer()
{
	if (_buffer.size() - _chunk_cursor > MAX_TRAILER_SIZE)
	{
		_has_error = true;   // trailers demais sem fechar — proteção
		return false;
	}
	size_t crlf = _buffer.find("\r\n", _chunk_cursor);
	if (crlf == std::string::npos)
		return false;

	if (crlf == _chunk_cursor)
	{
		_chunk_cursor += 2;
		_is_complete = true;
		return false;
	}

	size_t end = _buffer.find("\r\n\r\n", _chunk_cursor);
	if (end == std::string::npos)
		return false;
	_chunk_cursor = end + 4;
	_is_complete = true;
	return false;
}

void				HttpRequestBuilder::sendBadRequest() const
	{
			HttpRequest req = build();
			HttpResponse res(connection);
			Router router(_req, res, global_config);
			if (_error_status == 413)
				router.error.contentLarge(_error_message);
			else
				router.error.badRequest(_error_message);
	}

