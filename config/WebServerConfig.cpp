#include "WebServerConfig.hpp"

void WebServerConfig::addChild(ConfigNode* child)
{
    GlobalConfig* _child = dynamic_cast<GlobalConfig*>(child);
    if (_child)
        _global = _child;
    else
        addError("WebServerConfig: filho inválido, esperado GlobalConfig");
}
