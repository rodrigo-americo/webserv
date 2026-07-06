#include "ServerConfig.hpp"

void ServerConfig::addChild(ConfigNode* child)
{
    LocationConfig* location = dynamic_cast<LocationConfig*>(child);
    if (location)
    {
		location->setParent(this);
        _locations.push_back(location);
        return;
    }
    addError("ServerConfig: filho inválido, esperado LocationConfig");
}

const LocationConfig* ServerConfig::match_location(const std::string& uri) const
{
    const LocationConfig* best = NULL;
    size_t best_len = 0;

    for (std::list<LocationConfig*>::const_iterator it = _locations.begin(); it != _locations.end(); ++it)
    {
        if (!(*it)->matches(uri))
            continue;
        size_t len = (*it)->getPath().getCleanPath().string().size();
        if (!best || len > best_len)
        {
            best = *it;
            best_len = len;
        }
    }
    return (best);
}
