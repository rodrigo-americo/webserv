#include "ConfigInheritanceResolver.hpp"
#include "WebServerConfig.hpp"
#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "Path.hpp"

void ConfigInheritanceResolver::resolve()
{
    // const std::list<ServerConfig*>& servers = config->getServers();
	const std::list<ServerConfig*>& servers = WebServerConfig::getServers();
    for (std::list<ServerConfig*>::const_iterator it = servers.begin(); it != servers.end(); ++it)
        resolveServer(*it);
}

void ConfigInheritanceResolver::resolveServer(ServerConfig* server)
{
    if (server->getMethods().empty())
    {
        server->addMethod(GET);
        server->addMethod(POST);
        server->addMethod(DELETE);
    }
    const std::list<LocationConfig*>& locations = server->getLocations();
    for (std::list<LocationConfig*>::const_iterator it = locations.begin(); it != locations.end(); ++it)
        resolveLocation(*it, server);

    LocationConfig* defaultLoc = new LocationConfig();
    defaultLoc->setPath(Path(utils::str("/")));
    defaultLoc->setParent(server);
    resolveLocation(defaultLoc, server);
    server->setDefaultLocation(defaultLoc);
}

void ConfigInheritanceResolver::resolveLocation(LocationConfig* location, const ServerConfig* server)
{
    if (!location->getRoot().isNormalizable())
        location->setRoot(server->getRoot());

    if (!location->isAutoindexSet())
        location->setAutoindex(server->getAutoIndex());

    if (!location->isRequireAuthSet())
        location->setRequireAuth(server->getRequireAuth());

    if (location->getIndex().empty())
    {
        const std::list<std::string>& idx = server->getIndex();
        for (std::list<std::string>::const_iterator iit = idx.begin(); iit != idx.end(); ++iit)
            location->addIndex(*iit);
    }

    if (location->getMethods().empty())
    {
        const std::list<HttpMethod>& methods = server->getMethods();
        for (std::list<HttpMethod>::const_iterator mit = methods.begin(); mit != methods.end(); ++mit)
            location->addMethod(*mit);
    }

    if (location->getUploadDir().empty())
        location->setUploadDir(server->getUploadDir());

    const std::map<std::string, std::string>& serverCgi = server->getCgiExtensions();
    for (std::map<std::string, std::string>::const_iterator cit = serverCgi.begin(); cit != serverCgi.end(); ++cit)
    {
        if (location->getCgiExtensions().find(cit->first) == location->getCgiExtensions().end())
            location->addCgiExtension(*cit);
    }
}
