/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 20:45:14 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/07/03 22:01:10 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTER_HPP
# define ROUTER_HPP

# include "HttpResponseError.hpp"
# include "HttpRequest.hpp"
# include "HttpResponse.hpp"
# include "WebServerConfig.hpp"
# include "ServerConfig.hpp"
# include "LocationConfig.hpp"
# include "HttpError.hpp"

class Router
{
private:

	const WebServerConfig	*_config_global;

public:
	const HttpRequest		&req;
	HttpResponse			&res;
	const ServerConfig		*config_server;
	const LocationConfig	*config_location;
	HttpError	error;
	Router(const HttpRequest &req, HttpResponse &res, const WebServerConfig *config)
		: _config_global(config), req(req), res(res),
		config_server(config->match_server(req.port, req.headers.host())),
		config_location(config_server ? config_server->match_location(req.path.getPath().string()) : NULL),
		error(*this) {};
	~Router() {};


};

#endif
