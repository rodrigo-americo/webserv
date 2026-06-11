/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 02:13:29 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/10 23:21:58 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <unordered_map>
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
		typedef RequestMethod::type 							Method;
		Method		method;
		std::string	path;
		std::string	http_version;
		std::string	body;
		HttpHeaders	headers;

		HttpRequest(SocketConnection *conn)
			: _connection(conn), method(), path(), http_version(), body(), headers() {}

};

#endif
