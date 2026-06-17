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
	const std::list<ServerConfig*>& servers = getServers();
	for (std::list<ServerConfig*>::const_iterator it = servers.begin(); it != servers.end();it++){
		ServerConfig* server = *it;
		const std::list<ConfigServerListen>& listens = server->getListen();
		for (std::list<ConfigServerListen>::const_iterator lit = listens.begin(); lit != listens.end();lit++){
			if (lit->port == port){
				if(fallback == NULL)
					fallback = server;
				const std::list<std::string>& names = server->getServerNames();
				for (std::list<std::string>::const_iterator nit = names.begin(); nit != names.end(); ++nit)
				{
					if (*nit == host) return server;
				}
			}
		}
	}
	return (fallback);
}
