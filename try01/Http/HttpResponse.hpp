/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 02:14:21 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/29 17:28:02 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_RESPONSE_HPP
# define HTTP_RESPONSE_HPP

# include "SocketConnection.hpp"
# include "HttpHeaders.hpp"
# include "HttpResponseConversor_1_1.hpp"

struct ResponseHTTPVersion
{
	enum type
	{
		HTTP_1_0,
		HTTP_1_1,
		// impossivel para esse projeto
		HTTP_2_0,
		// impossivel para esse projeto
		HTTP_3_0,
	};
};


class HttpResponse
{
	private:
		SocketConnection	*_connection;
		std::string			_body;

	public:
		HttpHeaders			headers;
		int					status_code;
		std::string			status_code_message;
		HttpResponse(SocketConnection *conn): _connection(conn), _body(), headers(), status_code(500), status_code_message("Internal Server Error.") {}
		~HttpResponse() {}

		// void from(const std::string &str)
		// {
			
		// }
		
		int getFd() const { return _connection->fd(); }

		SocketConnection	*getConn() const { return _connection; }
		
		void	statusCode(int status, const std::string &msg)
		{
			status_code = status;
			status_code_message = msg;
		}

		void	body(const std::string &body)
		{
			_body = body;
			headers.content_length(utils::to_string(body.size()));
		}

		const std::string	&body() const { return _body; }

		void	send(ResponseHTTPVersion::type http_version)
		{
			std::string	response;
			HttpResponseConversor	*consersor = NULL;
			// convert headers and body to a http version
			switch (http_version)
			{
			case ResponseHTTPVersion::HTTP_1_0 :
				// HTTP_1_0_CONVERTER(*this, response);
				break;
			case ResponseHTTPVersion::HTTP_1_1 :
				consersor = new HttpResponseConversor_1_1(*this);
				break;

			default:
				std::cerr << "Error on send request!" << std::endl;
				break;
			}
			if (!consersor) return;
			consersor->convert(response);
			_connection->write(response);
		}
};

#endif
