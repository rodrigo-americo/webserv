#include "LocationConfig.hpp"
#include "ServerConfig.hpp"

bool LocationConfig::matches(const std::string& uri) const
{
    const std::string p = _path.getCleanPath().string();
    if (uri.compare(0, p.size(), p) != 0)
        return (false);
    if (uri.size() == p.size())
        return (true);
    return (uri[p.size()] == '/' || (!p.empty() && p[p.size() - 1] == '/'));
}

const Path &LocationConfig::resolveRoot() const {
    if (_root.isNormalizable() || !_parent)
        return (_root);
    return (_parent->getRoot());
}