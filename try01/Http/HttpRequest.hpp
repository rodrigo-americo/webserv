/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 02:13:29 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/29 12:21:30 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQUEST_HPP
# define HTTP_REQUEST_HPP

# include <string>

# include "SocketConnection.hpp"
# include "HttpHeaders.hpp"

struct RequestMethod
{
	enum type
	{
		GET,
		POST,
		PUT,
		PATCH,
		DELETE
	};
};

static const char *RequestMethodStr[] = {
	"GET",
	"POST",
	"PUT",
	"PATCH",
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
		std::string	path;
		std::string	http_version;
		std::string	body;
		HttpHeaders	headers;
		uint16_t port;

		HttpRequest(SocketConnection *conn)
			: _connection(conn), method(), path(), http_version(),
			 body(), headers(), port(conn->listenner()->addr().port()) {}
		SocketConnection *connection() const { return _connection; }

};

#endif
