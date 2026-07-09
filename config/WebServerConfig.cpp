#include "WebServerConfig.hpp"

void WebServerConfig::addChild(ConfigNode* child)
{
    GlobalConfig* _child = dynamic_cast<GlobalConfig*>(child);
    if (_child)
        _global = _child;
    else
        addError("WebServerConfig: filho inválido, esperado GlobalConfig");
}


const ServerConfig* WebServerConfig::match_server(size_t port, const std::string& host_header) const{
	size_t colon = host_header.find(':');
	std::string host = (colon != std::string::npos) ? host_header.substr(0, colon) : host_header;
	ServerConfig* fallback = NULL;
	ServerConfig* default_server = NULL;
	const std::list<ServerConfig*>& servers = getServers();
	for (std::list<ServerConfig*>::const_iterator it = servers.begin(); it != servers.end(); it++){
		ServerConfig* server = *it;
		const std::list<ConfigServerListen>& listens = server->getListen();
		for (std::list<ConfigServerListen>::const_iterator lit = listens.begin(); lit != listens.end(); lit++){
			if (lit->port != port)
				continue;
			if (fallback == NULL)
				fallback = server;
			if (lit->default_server && default_server == NULL)
				default_server = server;
			const std::list<std::string>& names = server->getServerNames();
			for (std::list<std::string>::const_iterator nit = names.begin(); nit != names.end(); ++nit){
				if (*nit == host)
					return server;
			}
		}
	}
	return (default_server != NULL) ? default_server : fallback;
}