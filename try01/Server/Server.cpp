#include <fstream>
#include <sstream>

#include "Server.hpp"
#include "Logger.hpp"
#include "HttpResponseError.hpp"
# include "Router.hpp"
#include "SessionManager.hpp"

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

void Server::_dispatch(Router &router)
{
    const std::map<std::string, std::string>& cgi_ext = router.config_location->getCgiExtensions();
    if (!cgi_ext.empty())
    {
        if (cgi_ext.count(router.req.path.getExtension().string()))
            return _serveCgi2(router);
    }
    if (router.req.method == RequestMethod::POST && !router.config_location->getUploadDir().empty())
        return _serveUpload2(router);
    if (router.req.method == RequestMethod::DELETE && !router.config_location->getUploadDir().empty())
        return _serveDelete(router);
    _serveStatic(router);
}

void Server::handleRequest(const HttpRequest &req, HttpResponse &res)
{
	LOG_INFO("route handler.");
	Router	router(req, res, _config);

	if (!router.config_server)
		return router.error.internalServerError();
	LOG_INFO("server config ok.");
    if (!router.config_location)
    {
		LOG_ERROR("Location not found");
        return router.error.notFound();
    }
	LOG_INFO("location config ok.");
	const std::list<HttpMethod>& methods = router.config_location->getMethods();
	if (!methods.empty() && !_methodAllowed(req.method, methods))
		return router.error.methodNotAllowed();
	LOG_INFO("methods allowed.");
	size_t max = router.config_server->getClientMaxBodySize();
	if (max > 0 && req.body.size() > max)
		return router.error.contentLarge();
	LOG_INFO("content length ok.");

    const std::pair<int, std::string>& redir = router.config_location->getRedirect();
    if (!redir.second.empty())
    {
		LOG_INFO("sending redirect.");
        res.statusCode(redir.first, "Moved");
        res.headers["Location"] = redir.second;
        res.send(ResponseHTTPVersion::HTTP_1_1);
        return;
    }
    if (req.path.getCleanPath() == "/login")
    {
        std::string id = SessionManager::getInstance().create();
        if (id.size() == 0)
            return router.error.internalServerError();
        res.statusCode(200, "OK");
        res.headers["Set-Cookie"] = "session_id=" + id;
        res.body("Login realizado.");
        res.send(ResponseHTTPVersion::HTTP_1_1);
        return;
    }
    if (router.config_location->getRequireAuth())
    {
        std::string session_id = req.headers.getCookieValue("session_id");
        if (session_id.empty() || !SessionManager::getInstance().isValid(session_id))
            return router.error.forbiden();
    }
    _dispatch(router);
}

