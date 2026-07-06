

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

void Server::_serveCgi(const Router &router)
{
    Path script_path = Path(router.config_location->resolveRoot()) + router.req.path;

    const std::map<std::string, std::string>& cgi_map = router.config_location->getCgiExtensions();
    std::map<std::string, std::string>::const_iterator it = cgi_map.find(router.req.path.getExtension().string());
    if (it == cgi_map.end())
        return router.error.internalServerError();
    std::string interpreter = it->second;
    FileSystem fs(script_path.getCleanPath());
    if (!fs.exists())
        return router.error.notFound();
    if (!fs.isFile())
        return router.error.forbiden();

    std::vector<std::string> env = _buildCgiEnv(router.req, *router.config_server, script_path.getCleanPath().string(), router.req.path.getCleanPath().string(), router.req.path.getQueryString().string());

    int fds_stdin[2];
    int fds_stdout[2];
    if (pipe(fds_stdin) == -1)
        return router.error.internalServerError();
    if (pipe(fds_stdout) == -1)
    {
        close(fds_stdin[0]);
        close(fds_stdin[1]);
        return router.error.internalServerError();
    }
    int pid_child = fork();
    if (pid_child == -1)
    {
        close(fds_stdin[0]);
        close(fds_stdin[1]);
        close(fds_stdout[0]);
        close(fds_stdout[1]);
        return router.error.internalServerError();
    }
    else if(pid_child == 0)
    {
        if (close(fds_stdin[1]) == -1 || close(fds_stdout[0]) == -1)
            _exit(1);
        int stdin_process = dup2(fds_stdin[0], 0);
        if (stdin_process == -1)
            _exit(1);
        int stdout_process = dup2(fds_stdout[1], 1);
        if (stdout_process == -1)
            _exit(1);
        close(fds_stdin[0]);
        close(fds_stdout[1]);

		LOG_TRACE("CHILD_PROCESS: execute CGI CD to " << script_path.getLastDir().c_str());
        if (chdir(script_path.getLastDir().c_str()) == -1)
            _exit(1);
        utils::str filename = router.req.path.getFilename();
        std::vector<char *> argv;
        argv.push_back(const_cast<char *>(interpreter.c_str()));
        argv.push_back(const_cast<char *>(const_cast<char *>(filename.c_str())));
        argv.push_back(NULL);
        std::vector<char *> envp;
        for (size_t i = 0; i < env.size(); ++i)
            envp.push_back(const_cast<char *>(env[i].c_str()));
        envp.push_back(NULL);
		// LOG_TRACE("execve CGI child process.");
        execve(interpreter.c_str(), &argv[0], &envp[0]);
		// LOG_ERROR("execve CGI Error.");
        _exit(1);
    }
    else
    {
        close(fds_stdin[0]);
        close(fds_stdout[1]);
        std::string output;
        SocketPipeWrite *stdin_pipe  = new SocketPipeWrite(fds_stdin[1]);
        SocketPipeRead  *stdout_pipe = new SocketPipeRead(fds_stdout[0]);
        if (stdin_pipe->hasErrors() || stdout_pipe->hasErrors())
        {
            delete stdin_pipe;
            delete stdout_pipe;
            ::kill(pid_child, SIGKILL);
            waitpid(pid_child, NULL, 0);
            return router.error.internalServerError();
        }
        CgiProcess *cgi = new CgiProcess(router.res.getConn(), router.req, stdin_pipe, stdout_pipe, pid_child);
        ConnectionPool::getInstance().addCgi(cgi);
    }
}

void Server::_serveCgi2(Router &router)
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
