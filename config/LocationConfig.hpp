#ifndef LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP

# include <string>
# include <list>
# include <map>
# include "ConfigNode.hpp"
# include "http.hpp"

class LocationConfig: public ConfigLeaf{
    private:
        LocationConfig(const LocationConfig &other);
        LocationConfig &operator=(const LocationConfig &other);
        std::string _path;
        LocationModifier _modifier;
        std::list<HttpMethod> _methods;
        std::string _redirect;
        std::string _root;
        bool _autoindex;
        std::list<std::string> _index;
        std::list<std::string> _try_files;
        std::string _upload_dir;
        std::string _proxy_pass;
        std::string _proxy_cache_bypass;
        std::string _fastcgi_index;
        std::string _expires;
        std::list<std::pair<std::string, std::string> > _add_header;
        std::list<std::pair<std::string, std::string> > _proxy_set_header;
        std::map<std::string, std::string> _cgi;
    public:
        LocationConfig() : _modifier(MOD_NONE), _autoindex(false) {}
        void setPath(const std::string& path)                          { _path = path; }
        void setModifier(LocationModifier modifier)                    { _modifier = modifier; }
        void setRedirect(const std::string& redirect)                  { _redirect = redirect; }
        void setRoot(const std::string& root)                          { _root = root; }
        void setAutoindex(bool autoindex)                              { _autoindex = autoindex; }
        void setUploadDir(const std::string& dir)                      { _upload_dir = dir; }
        void setProxyPass(const std::string& v)                        { _proxy_pass = v; }
        void setProxyCacheBypass(const std::string& v)                 { _proxy_cache_bypass = v; }
        void setFastcgiIndex(const std::string& v)                     { _fastcgi_index = v; }
        void setExpires(const std::string& v)                          { _expires = v; }
        const std::string &getPath() const                             { return _path; }
        LocationModifier getModifier() const                           { return _modifier; }
        const std::string &getRedirect() const                         { return _redirect; }
        const std::string &getRoot() const                             { return _root; }
        bool getAutoIndex() const                                      { return _autoindex; }
        const std::string &getUploadDir() const                        { return _upload_dir; }
        const std::string &getProxyPass() const                        { return _proxy_pass; }
        const std::string &getProxyCacheBypass() const                 { return _proxy_cache_bypass; }
        const std::string &getFastcgiIndex() const                     { return _fastcgi_index; }
        const std::string &getExpires() const                          { return _expires; }
        const std::list<HttpMethod> &getMethods() const                { return _methods; }
        const std::list<std::string> &getIndex() const                 { return _index; }
        const std::list<std::string> &getTryFiles() const              { return _try_files; }
        const std::list<std::pair<std::string,std::string> > &getAddHeader() const       { return _add_header; }
        const std::list<std::pair<std::string,std::string> > &getProxySetHeader() const  { return _proxy_set_header; }
        const std::map<std::string, std::string> &getCgi() const       { return _cgi; }
        void addMethod(HttpMethod method)                              { _methods.push_back(method); }
        void addIndex(const std::string& index)                        { _index.push_back(index); }
        void addTryFile(const std::string& v)                          { _try_files.push_back(v); }
        void addAddHeader(const std::string& k, const std::string& v)  { _add_header.push_back(std::make_pair(k, v)); }
        void addProxySetHeader(const std::string& k, const std::string& v) { _proxy_set_header.push_back(std::make_pair(k, v)); }
        void addCgi(const std::pair<const std::string, const std::string>& cgi) { _cgi[cgi.first] = cgi.second; }
        bool matches(const std::string &uri) const;
};

#endif