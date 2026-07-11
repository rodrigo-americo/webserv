#include "GlobalConfig.hpp"

void GlobalConfig::addChild(ConfigNode* child)
{
    HttpConfig* http = dynamic_cast<HttpConfig*>(child);
    if (http)
    {
        _http = http;
        return;
    }
    EventsConfig* events = dynamic_cast<EventsConfig*>(child);
    if (events)
    {
        _events = events;
        return;
    }
    addError("GlobalConfig: filho inválido, esperado HttpConfig ou EventsConfig");
}
