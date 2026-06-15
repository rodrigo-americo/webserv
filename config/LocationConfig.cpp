#include "LocationConfig.hpp"

bool LocationConfig::matches(const std::string& uri) const
{
    if (_modifier == MOD_NONE || _modifier == MOD_PREFIX)
        return ( uri.substr(0, _path.size()) == _path);
    else if (_modifier == MOD_EXACT)
        return (uri == _path);
    else
        return (false);
}
