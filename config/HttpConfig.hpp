#ifndef HTTPCONFIG_HPP
# define HTTPCONFIG_HPP

# include <list>
# include "ConfigNode.hpp"
# include "ServerConfig.hpp"

class HttpConfig: public ConfigContainer{
    private:
        HttpConfig(const HttpConfig &other);
        HttpConfig &operator=(const HttpConfig &other);
        size_t      _keepalive_timeout;
        std::list<ServerConfig*>   _servers;
    public:
        HttpConfig() : _keepalive_timeout(0) {}
        ~HttpConfig() {
            for (std::list<ServerConfig*>::iterator it = _servers.begin(); it != _servers.end(); ++it)
                delete *it;
        }

        void setKeepaliveTimeout(size_t timeout)            { _keepalive_timeout = timeout; }

        size_t                           getKeepaliveTimeout() const  { return _keepalive_timeout; }
        const std::list<ServerConfig*>&  getServers() const           { return _servers; }
        void addChild(ConfigNode* child);
};

#endif
