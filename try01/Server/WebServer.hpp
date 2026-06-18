#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# include <vector>
# include <list>
# include "WebServerConfig.hpp"
# include "IMultiplexer.hpp"
# include "Server.hpp"
# include "Socket.hpp"

class WebServer {
private:
	IMultiplexer*        _multiplexer;
	std::vector<Server*> _servers;
	std::vector<Socket*> _listeners;

	IMultiplexer* _createMultiplexer(const WebServerConfig* config);

public:
	WebServer();
	~WebServer();
	void start(WebServerConfig* config);
};

#endif