#ifndef CONFIGINHERITANCERESOLVER_HPP
# define CONFIGINHERITANCERESOLVER_HPP

class WebServerConfig;
class ServerConfig;
class LocationConfig;

class ConfigInheritanceResolver {
private:
    static void resolveServer(ServerConfig* server);
    static void resolveLocation(LocationConfig* location, const ServerConfig* server);
public:
    static void resolve(WebServerConfig* config);
};

#endif