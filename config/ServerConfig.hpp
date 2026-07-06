#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include <string>
# include <list>
# include <map>
# include "ConfigNode.hpp"
# include "ConfigServerListen.hpp"
# include "LocationConfig.hpp"
# include "Path.hpp" 
# include "http.hpp"

class ServerConfig: public ConfigContainer{
    private:
        ServerConfig(const ServerConfig &other);
        ServerConfig &operator=(const ServerConfig &other);
        std::list<ConfigServerListen> _listen;
		Path _root;
        std::list<std::string> _server_names;
        std::map<int, std::string> _error_page;
        size_t _client_max_body_size;
        std::list<LocationConfig*> _locations;
    public:
        ServerConfig() : _client_max_body_size(0){}
        ~ServerConfig() {
            for (std::list<LocationConfig*>::iterator it = _locations.begin(); it != _locations.end(); ++it)
                delete *it;
        }
        void setClientMaxBodySize(size_t size)					{ _client_max_body_size = size; }
        void addListen(const ConfigServerListen& listen)		{ _listen.push_back(listen); }
        void setRoot(const Path &root)					        { _root=root;}
		void addServerName(const std::string& name)				{ _server_names.push_back(name); }
        void addErrorPage(int code, const std::string& path)	{ _error_page[code] = path; }

        size_t									getClientMaxBodySize() const { return _client_max_body_size; }
        const std::list<ConfigServerListen>&	getListen() const            { return _listen; }
        const Path& 						    getRoot() const 			 { return _root;}
		const std::list<std::string>&			getServerNames() const       { return _server_names; }
        const std::map<int, std::string>&		getErrorPages() const        { return _error_page; }
        const std::list<LocationConfig*>&		getLocations() const         { return _locations; }
        const LocationConfig* match_location(const std::string& uri) const;
        void addChild(ConfigNode*);
};

#endif
