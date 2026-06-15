#ifndef HTTPCONFIG_HPP
# define HTTPCONFIG_HPP

# include <string>
# include <list>
# include "ConfigNode.hpp"
# include "ServerConfig.hpp"
# include "UpstreamConfig.hpp"

class HttpConfig: public ConfigContainer{
    private:
        HttpConfig(const HttpConfig &other);
        HttpConfig &operator=(const HttpConfig &other);
        std::string _include;
        size_t      _keepalive_timeout;
        bool        _sendfile;
        std::string _default_type;
        std::string _log_format;
        std::string _access_log;
        std::string _expires;
        std::list<ServerConfig*>   _servers;
        std::list<UpstreamConfig*> _upstreams;
    public:
        HttpConfig() : _keepalive_timeout(0), _sendfile(false) {}
        ~HttpConfig() {
            for (std::list<ServerConfig*>::iterator it = _servers.begin(); it != _servers.end(); ++it)
                delete *it;
            for (std::list<UpstreamConfig*>::iterator it = _upstreams.begin(); it != _upstreams.end(); ++it)
                delete *it;
        }

        void setInclude(const std::string& include)        { _include = include; }
        void setKeepaliveTimeout(size_t timeout)            { _keepalive_timeout = timeout; }
        void setSendfile(bool v)                            { _sendfile = v; }
        void setDefaultType(const std::string& v)          { _default_type = v; }
        void setLogFormat(const std::string& v)            { _log_format = v; }
        void setAccessLog(const std::string& v)            { _access_log = v; }
        void setExpires(const std::string& v)              { _expires = v; }

        const std::string&               getInclude() const          { return _include; }
        size_t                           getKeepaliveTimeout() const  { return _keepalive_timeout; }
        bool                             getSendfile() const          { return _sendfile; }
        const std::string&               getDefaultType() const       { return _default_type; }
        const std::string&               getLogFormat() const         { return _log_format; }
        const std::string&               getAccessLog() const         { return _access_log; }
        const std::string&               getExpires() const           { return _expires; }
        const std::list<ServerConfig*>&  getServers() const           { return _servers; }
        const std::list<UpstreamConfig*>& getUpstreams() const        { return _upstreams; }

        void addChild(ConfigNode* child);
};

#endif
