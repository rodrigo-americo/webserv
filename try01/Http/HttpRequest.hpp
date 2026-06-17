/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 02:13:29 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/11 17:35:21 by bruno-valer      ###   ########.fr       */
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

		HttpRequest(SocketConnection *conn)
			: _connection(conn), method(), path(), http_version(), body(), headers() { (void)_connection; }

};

#endif
