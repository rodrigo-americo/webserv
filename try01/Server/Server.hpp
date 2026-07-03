/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 02:08:12 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/07/02 14:38:02 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

class Server
{
    private:
        const WebServerConfig* _config;

        void _sendError(HttpResponse &res, int code, const std::string &msg, const HttpRequest *req = NULL);
        bool _methodAllowed(HttpRequest::Method method, const std::list<HttpMethod>& allowed);
        void _dispatch(const HttpRequest &req, HttpResponse &res, const ServerConfig &server, const LocationConfig &location);
        void _serveCgi(const HttpRequest &req, HttpResponse &res, const LocationConfig &location, const ServerConfig &server);
        void _serveUpload(const HttpRequest &req, HttpResponse &res, const LocationConfig &location);
        void _serveDelete(const HttpRequest &req, HttpResponse &res, const LocationConfig &location);
        void _serveStatic(const HttpRequest &req, HttpResponse &res, const ServerConfig &server, const LocationConfig &location);
        void _serveAutoIndex(const HttpRequest &req, HttpResponse &res, const std::string &dir_path);
        std::vector<std::string> _buildCgiEnv(const HttpRequest &req, const ServerConfig &server, const std::string &script_path, const std::string &clean_path, const std::string &query_string);

    public:
        Server(const WebServerConfig* config) : _config(config) {}
        ~Server() {}

		const WebServerConfig *getConfig() const { return _config; }
        void handleRequest(const HttpRequest &req, HttpResponse &res);
        
};

#endif
