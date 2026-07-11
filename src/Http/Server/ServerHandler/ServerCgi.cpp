

#include <string>
#include <sys/stat.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "Server.hpp"
#include "SocketPipeWrite.hpp"
#include "SocketPipeRead.hpp"
#include "CgiProcess.hpp"
#include "ConnectionPool.hpp"
#include "Logger.hpp"
#include "Path.hpp"
#include "HttpResponseError.hpp"
#include "FileSystem.hpp"
#include "Router.hpp"
#include "Cgi.hpp"

void Server::_serveCgi(Router &router)
{
	router.cgi.createProcess();
}

static std::string _envEntry(const std::string &key, const std::string &value)
{
    return key + "=" + value;
}

static std::string _toHttpEnvKey(const std::string &header_name)
{
    std::string result = "HTTP_";
    for (size_t i = 0; i < header_name.size(); ++i)
    {
        char c = header_name[i];
        if (c == '-')
            result += '_';
        else if (c >= 'a' && c <= 'z')
            result += (c - 'a' + 'A');
        else
            result += c;
    }
    return result;
}

std::vector<std::string> Server::_buildCgiEnv(const HttpRequest &req, const ServerConfig &server,
    const std::string &script_path, const std::string &clean_path, const std::string &query_string)
{
    (void)server;
    std::vector<std::string> env_strings;
    env_strings.push_back(_envEntry("REQUEST_METHOD", RequestMethodStr[req.method]));
    env_strings.push_back(_envEntry("QUERY_STRING", query_string));
    env_strings.push_back(_envEntry("CONTENT_LENGTH", utils::to_string(req.body.size())));
    env_strings.push_back(_envEntry("CONTENT_TYPE", req.headers.content_type()));
    env_strings.push_back(_envEntry("SCRIPT_FILENAME", script_path));
    env_strings.push_back(_envEntry("SCRIPT_NAME", clean_path));
    env_strings.push_back(_envEntry("PATH_INFO", ""));
    env_strings.push_back(_envEntry("SERVER_PROTOCOL", "HTTP/1.1"));
    env_strings.push_back(_envEntry("GATEWAY_INTERFACE", "CGI/1.1"));
    env_strings.push_back(_envEntry("SERVER_NAME", req.headers.host()));
    env_strings.push_back(_envEntry("SERVER_PORT", utils::to_string(req.port)));
    env_strings.push_back(_envEntry("REDIRECT_STATUS", "200"));
    for (HttpHeaders::const_iterator it = req.headers.begin(); it != req.headers.end(); ++it)
    {
        if (it->first == "Content-Type" || it->first == "Content-Length")
            continue;
        env_strings.push_back(_envEntry(_toHttpEnvKey(it->first), it->second));
    }
    return env_strings;
}
