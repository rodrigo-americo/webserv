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
    const LocationConfig* best_prefix = NULL;
    const LocationConfig* best_none = NULL;

    for (std::list<LocationConfig*>::const_iterator it = _locations.begin(); it != _locations.end(); ++it)
    {
        if (!(*it)->matches(uri))
            continue;
        LocationModifier mod = (*it)->getModifier();
        if (mod == MOD_EXACT)
            return *it;
        if (mod == MOD_PREFIX)
        {
            if (!best_prefix || (*it)->getPath().size() > best_prefix->getPath().size())
                best_prefix = *it;
        }
        else
        {
            if (!best_none || (*it)->getPath().size() > best_none->getPath().size())
                best_none = *it;
        }
    }
    if (best_prefix)
        return best_prefix;
    return best_none;
}
