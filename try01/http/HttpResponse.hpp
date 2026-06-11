/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 02:14:21 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/10 23:33:54 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "SocketConnection.hpp"
# include "HttpHeaders.hpp"

struct ResponseHTTPVersion
{
	enum type
	{
		HTTP_1_0,
		HTTP_1_1,
		HTTP_2_0,
		HTTP_3_0,
	};
};


class HttpResponse
{
	private:
		SocketConnection	*_connection;
		int					_status_code;
		std::string			_status_code_message;

	public:
		HttpHeaders			headers;
		HttpResponse(SocketConnection *conn): _connection(conn), headers() {}
		~HttpResponse() {}

		void	statusCode(int status, const std::string &msg)
		{
			_status_code = status;
			_status_code_message = msg;
		}

		void	send(ResponseHTTPVersion::type http_version)
		{
			std::string	response;
			// convert headers and body to a http version
			switch (http_version)
			{
			case ResponseHTTPVersion::HTTP_1_0 :
				// HTTP_1_0_CONVERTER(*this, response);
				break;
			case ResponseHTTPVersion::HTTP_1_1 :
				// HTTP_1_1_CONVERTER(*this, response);
				break;
			case ResponseHTTPVersion::HTTP_2_0 :
				// HTTP_2_0_CONVERTER(*this, response);
				break;
			case ResponseHTTPVersion::HTTP_3_0 :
				// HTTP_3_0_CONVERTER(*this, response);
				break;

			default:
				std::cerr << "Error on send request!" << std::endl;
				break;
			}
			// so para teste
			response +=
			"HTTP/1.1 200 OK\r\n"
			"Content-Length: 13\r\n"
			"\r\n"
			"Hello, World!";
			_connection->write(response);
		}
};

#endif
