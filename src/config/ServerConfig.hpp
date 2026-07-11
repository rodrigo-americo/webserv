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
# include "optional.hpp"

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
        Optional<bool> _autoindex;
        Optional<bool> _require_auth;
        std::list<std::string> _index;
        std::list<HttpMethod> _methods;
        std::map<std::string, std::string> _cgi_extensions;
        std::string _upload_dir;
        LocationConfig* _default_location;
    public:
        ServerConfig() : _client_max_body_size(0), _default_location(NULL) {}
        ~ServerConfig() {
            for (std::list<LocationConfig*>::iterator it = _locations.begin(); it != _locations.end(); ++it)
                delete *it;
            delete _default_location;
        }
        void setClientMaxBodySize(size_t size)					{ _client_max_body_size = size; }
        void addListen(const ConfigServerListen& listen)		{ _listen.push_back(listen); }
        void setRoot(const Path &root)					        { _root=root;}
		void addServerName(const std::string& name)				{ _server_names.push_back(name); }
        void addErrorPage(int code, const std::string& path)	{ _error_page[code] = path; }
        void setAutoindex(bool value)      { _autoindex.set(value); }
        void setRequireAuth(bool value)    { _require_auth.set(value); }
        void addIndex(const std::string& str)      { _index.push_back(str); }
        void addMethod(HttpMethod method)             { _methods.push_back(method); }
        void addCgiExtension(const std::pair<const std::string, const std::string>& ext) { _cgi_extensions[ext.first] = ext.second; }
        void setUploadDir(const std::string& dir)  { _upload_dir = dir; }
        void setDefaultLocation(LocationConfig* loc) { delete _default_location; _default_location = loc; }

        size_t									getClientMaxBodySize() const { return _client_max_body_size; }
        const std::list<ConfigServerListen>&	getListen() const            { return _listen; }
        const Path& 						    getRoot() const 			 { return _root;}
		const std::list<std::string>&			getServerNames() const       { return _server_names; }
        const std::map<int, std::string>&		getErrorPages() const        { return _error_page; }
        const std::list<LocationConfig*>&		getLocations() const         { return _locations; }
        const LocationConfig* match_location(const std::string& uri) const;
        void addChild(ConfigNode*);
        bool getAutoIndex() const      { return _autoindex.isSet() && _autoindex.get(); }
        bool getRequireAuth() const    { return _require_auth.isSet() && _require_auth.get(); }
        bool isAutoindexSet() const    { return _autoindex.isSet(); }
        bool isRequireAuthSet() const  { return _require_auth.isSet(); }
        const std::list<std::string>& getIndex() const { return _index; }
        const std::list<HttpMethod>& getMethods() const { return _methods; }
        const std::map<std::string, std::string>& getCgiExtensions() const { return _cgi_extensions; }
        const std::string& getUploadDir() const { return _upload_dir; }
        const LocationConfig* getDefaultLocation() const { return _default_location; }  
};

#endif
