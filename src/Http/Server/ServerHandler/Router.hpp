#ifndef ROUTER_HPP
# define ROUTER_HPP

# include "Cgi.hpp"
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

public:
	const HttpRequest		&req;
	HttpResponse			&res;
	const ServerConfig		*config_server;
	const LocationConfig	*config_location;
	HttpError	error;
	Cgi			cgi;
	Router(const HttpRequest &req, HttpResponse &res)
		: req(req), res(res),
		config_server(WebServerConfig::match_server(req)),
		config_location(config_server ? config_server->match_location(req.path.getCleanPath().string()) : NULL),
		error(*this), cgi(*this)
		{};
	~Router() {};


};

#endif
