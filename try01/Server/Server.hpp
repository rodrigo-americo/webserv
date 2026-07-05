#ifndef SERVER_HPP
# define SERVER_HPP

# include <list>
# include <map>
# include <string>
# include "HttpRequest.hpp"
# include "HttpResponse.hpp"
# include "WebServerConfig.hpp"
# include "ServerConfig.hpp"
# include "LocationConfig.hpp"
# include "Router.hpp"

class Server
{
    private:
        const WebServerConfig* _config;

        bool _methodAllowed(HttpRequest::Method method, const std::list<HttpMethod>& allowed);
        void _dispatch(Router &router);
        void _serveCgi(const Router &router);
		void _serveCgi2(Router &router);
        void _serveUpload(const Router &router);
        void _serveDelete(const Router &router);
        void _serveStatic(const Router &router);
        void _serveAutoIndex(const Router &router, const FileSystem &fs);
        std::vector<std::string> _buildCgiEnv(const HttpRequest &req, const ServerConfig &server, const std::string &script_path, const std::string &clean_path, const std::string &query_string);

    public:
        Server(const WebServerConfig* config) : _config(config) {}
        ~Server() {}

		const WebServerConfig *getConfig() const { return _config; }
        void handleRequest(const HttpRequest &req, HttpResponse &res);

};

#endif
