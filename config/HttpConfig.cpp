#include "HttpConfig.hpp"

void HttpConfig::addChild(ConfigNode* child)
{
    ServerConfig* server = dynamic_cast<ServerConfig*>(child);
    if (server)
    {
        _servers.push_back(server);
        return;
    }
    UpstreamConfig* upstream = dynamic_cast<UpstreamConfig*>(child);
    if (upstream)
    {
        _upstreams.push_back(upstream);
        return;
    }
    addError("HttpConfig: filho inválido, esperado ServerConfig ou UpstreamConfig");
}
