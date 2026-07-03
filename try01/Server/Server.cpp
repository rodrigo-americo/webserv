#include <fstream>
#include <sstream>
#include "Server.hpp"
#include "Logger.hpp"
#include "HttpResponseError.hpp"

void Server::_sendError(HttpResponse &res, int code, const std::string &msg, const HttpRequest *req)
{
    res.statusCode(code, msg);
    if (req && _config)
    {
        const ServerConfig* server = _config->match_server(req->port, req->headers.host());
        if (server)
        {
            const std::map<int, std::string>& pages = server->getErrorPages();
            std::map<int, std::string>::const_iterator it = pages.find(code);
            if (it != pages.end())
            {
                std::ifstream file(it->second.c_str());
                if (file.is_open())
                {
                    std::ostringstream ss;
                    ss << file.rdbuf();
                    res.body(ss.str());
                    res.send(ResponseHTTPVersion::HTTP_1_1);
                    return;
                }
            }
        }
    }
    res.body(msg + "\n");
    res.send(ResponseHTTPVersion::HTTP_1_1);
}

bool Server::_methodAllowed(HttpRequest::Method method, const std::list<HttpMethod>& allowed)
{
    for (std::list<HttpMethod>::const_iterator it = allowed.begin(); it != allowed.end(); ++it)
    {
        if (*it == GET    && method == RequestMethod::GET)    return true;
        if (*it == POST   && method == RequestMethod::POST)   return true;
        if (*it == DELETE && method == RequestMethod::DELETE) return true;
    }
    return false;
}

void Server::_dispatch(const HttpRequest &req, HttpResponse &res,
               const ServerConfig &server, const LocationConfig &location)
{
    const std::map<std::string, std::string>& cgi_ext = location.getCgiExtensions();
    if (!cgi_ext.empty())
    {
        if (cgi_ext.count(req.path.getExtension().string()))
            return _serveCgi(req, res, location, server);
    }
    if (req.method == RequestMethod::POST && !location.getUploadDir().empty())
        return _serveUpload(req, res, location);
    if (req.method == RequestMethod::DELETE && !location.getUploadDir().empty())
        return _serveDelete(req, res, location);
    _serveStatic(req, res, server, location);
}

void Server::handleRequest(const HttpRequest &req, HttpResponse &res)
{
    const ServerConfig* server = _config->match_server(req.port, req.headers.host());
	return HttpResponseError(res, 400, "Internal Server Error", server).send(ResponseHTTPVersion::HTTP_1_1);
    if (!server)
        return _sendError(res, 500, "Internal Server Error");

    const LocationConfig* location = server->match_location(req.path.getPath().string());
    if (!location)
    {
        std::cout << "Location not found \n";
        return _sendError(res, 404, "Not Found", &req);
    }

    const std::list<HttpMethod>& methods = location->getMethods();
    if (!methods.empty() && !_methodAllowed(req.method, methods))
        return _sendError(res, 405, "Method Not Allowed", &req);

    size_t max = server->getClientMaxBodySize();
    if (max > 0 && req.body.size() > max)
        return _sendError(res, 413, "Content Too Large", &req);

    const std::pair<int, std::string>& redir = location->getRedirect();
    if (!redir.second.empty())
    {
        res.statusCode(redir.first, "Moved");
        res.headers["Location"] = redir.second;
        res.send(ResponseHTTPVersion::HTTP_1_1);
        return;
    }
    _dispatch(req, res, *server, *location);
}

