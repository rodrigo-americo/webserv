#ifndef LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP

# include <string>
# include <list>
# include <map>
# include "Path.hpp"
# include "ConfigNode.hpp"
# include "http.hpp"

class ServerConfig;

class LocationConfig: public ConfigLeaf{
    private:
        LocationConfig(const LocationConfig &other);
        LocationConfig &operator=(const LocationConfig &other);
        Path _path;
        std::list<HttpMethod> _methods;
        std::pair<int, std::string> _redirect;
        Path _root;
        bool _autoindex;
        std::list<std::string> _index;
        std::string _upload_dir;
        std::map<std::string, std::string> _cgi_extensions;
		const ServerConfig* _parent;
    public:
        LocationConfig() : _redirect(0, ""), _autoindex(false), _parent(NULL) {}
        void setPath(const Path& path)                                 { _path = path; }
        void setRedirect(int code,const std::string& url)              { _redirect = std::make_pair(code, url); }
        void setRoot(const  Path& root)                                { _root = root; }
        void setAutoindex(bool autoindex)                              { _autoindex = autoindex; }
        void setUploadDir(const std::string& dir)                      { _upload_dir = dir; }
        void setParent(const ServerConfig* parent)                     {_parent = parent;}
		const Path &getPath() const                                    { return _path; }
        const std::pair<int, std::string> &getRedirect() const         { return _redirect; }
        const Path &getRoot() const                                    { return _root; }
        bool getAutoIndex() const                                      { return _autoindex; }
        const std::string &getUploadDir() const                        { return _upload_dir; }
        const std::list<HttpMethod> &getMethods() const                { return _methods; }
        const std::list<std::string> &getIndex() const                 { return _index; }
        const std::map<std::string, std::string> &getCgiExtensions() const       { return _cgi_extensions; }
		const Path &resolveRoot() const;
		void addMethod(HttpMethod method)                              { _methods.push_back(method); }
        void addIndex(const std::string& index)                        { _index.push_back(index); }
        void addCgiExtension(const std::pair<const std::string, const std::string>& cgi_extension) { _cgi_extensions[cgi_extension.first] = cgi_extension.second; }
        bool matches(const std::string &uri) const;
};

#endif
