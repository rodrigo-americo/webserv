
#ifndef HTTP_REQUEST_HPP
# define HTTP_REQUEST_HPP

# include <string>

# include "SocketConnection.hpp"
# include "HttpHeaders.hpp"
# include "Path.hpp"

struct RequestMethod
{
	enum type
	{
		GET,
		POST,
		DELETE
	};
};

static const char *RequestMethodStr[] = {
	"GET",
	"POST",
	"DELETE"
};

inline std::ostream	&operator<<(std::ostream &os, RequestMethod::type req)
{
	os << RequestMethodStr[req];
	return os;
}

class HttpRequest
{
	private:
		SocketConnection	*_connection;

	public:
		typedef RequestMethod::type		Method;
		Method		method;
		Path		path;
		std::string	http_version;
		std::string	body;
		HttpHeaders	headers;
		uint16_t	port;

		HttpRequest(SocketConnection *conn)
			: _connection(conn), method(), path(utils::str("")), http_version(),
			 body(), headers(), port(conn->listenner()->addr().port()) {}
		SocketConnection *connection() const { return _connection; }

};

#endif
