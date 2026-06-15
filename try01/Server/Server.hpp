/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 02:08:12 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/11 17:42:16 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "HttpRequest.hpp"
# include "HttpResponse.hpp"

class Server
{
private:


public:
	Server() {}
	~Server() {}

	void	handleRequest(const HttpRequest &req, HttpResponse &res)
	{
		(void)req;
		res.statusCode(200, "OK");
		if (!req.body.empty())
			res.body("Voce enviou:\n\n" + req.body);
		else
			res.body("Hello, World!\n\nEu sou o Bruno!");
		res.send(ResponseHTTPVersion::HTTP_1_1);
	}
};

#endif
