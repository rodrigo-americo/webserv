#include "HttpConfig.hpp"

void HttpConfig::addChild(ConfigNode* child)
{
    ServerConfig* server = dynamic_cast<ServerConfig*>(child);
    if (server)
    {
        _servers.push_back(server);
        return;
    }
    addError("HttpConfig: filho inválido, esperado ServerConfig");
}
