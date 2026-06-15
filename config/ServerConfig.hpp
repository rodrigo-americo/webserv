#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <string>
# include <list>
# include <map>
# include "ConfigNode.hpp"
# include "LocationConfig.hpp"
# include "http.hpp"

struct Listen {
    std::string host;
    int port;
};

class ServerConfig: public ConfigContainer{
    private:
        ServerConfig(const ServerConfig &other);
        ServerConfig &operator=(const ServerConfig &other);
        std::list<Listen> _listen;
        std::list<std::string> _server_names;
        std::map<int, std::string> _error_page;
        size_t _client_max_body_size;
        std::string _access_log;
        std::string _ssl_certificate;
        std::string _ssl_certificate_key;
        std::string _ssl_protocols;
        std::string _ssl_ciphers;
        std::list<LocationConfig*> _locations;
    public:
        ServerConfig() : _client_max_body_size(0) {}
        ~ServerConfig() {
            for (std::list<LocationConfig*>::iterator it = _locations.begin(); it != _locations.end(); ++it)
                delete *it;
        }
        void setClientMaxBodySize(size_t size)               { _client_max_body_size = size; }
        void addListen(const Listen& listen)                 { _listen.push_back(listen); }
        void addServerName(const std::string& name)          { _server_names.push_back(name); }
        void addErrorPage(int code, const std::string& path) { _error_page[code] = path; }
        void setAccessLog(const std::string& v)              { _access_log = v; }
        void setSslCertificate(const std::string& v)         { _ssl_certificate = v; }
        void setSslCertificateKey(const std::string& v)      { _ssl_certificate_key = v; }
        void setSslProtocols(const std::string& v)           { _ssl_protocols = v; }
        void setSslCiphers(const std::string& v)             { _ssl_ciphers = v; }

        size_t                                    getClientMaxBodySize() const { return _client_max_body_size; }
        const std::list<Listen>&                  getListen() const            { return _listen; }
        const std::list<std::string>&             getServerNames() const       { return _server_names; }
        const std::map<int, std::string>&         getErrorPages() const        { return _error_page; }
        const std::string&                        getAccessLog() const         { return _access_log; }
        const std::string&                        getSslCertificate() const    { return _ssl_certificate; }
        const std::string&                        getSslCertificateKey() const { return _ssl_certificate_key; }
        const std::string&                        getSslProtocols() const      { return _ssl_protocols; }
        const std::string&                        getSslCiphers() const        { return _ssl_ciphers; }
        const std::list<LocationConfig*>&         getLocations() const         { return _locations; }

        const LocationConfig* match_location(const std::string& uri) const;
        void addChild(ConfigNode*);
};

#endif