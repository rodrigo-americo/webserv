/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 02:08:12 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/10 21:55:37 by bruno-valer      ###   ########.fr       */
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
		(void)res;
	}
};

#endif
