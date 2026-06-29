/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCgi.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/28 21:08:03 by ighannam          #+#    #+#             */
/*   Updated: 2026/06/29 17:29:17 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


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

void Server::_serveCgi(const HttpRequest &req, HttpResponse &res,
                        const LocationConfig &location, const ServerConfig &server)
{
    std::string clean_path = req.path;
    std::string query_string;
    size_t qpos = req.path.find('?');
    if (qpos != std::string::npos)
    {
        clean_path = req.path.substr(0, qpos);
        query_string = req.path.substr(qpos + 1);
    }
    
    std::string root = location.getRoot().empty() ? server.getRoot() : location.getRoot();

    if (!root.empty() && root[root.size() - 1] == '/')
        root = root.substr(0, root.size() - 1);

    std::string script_path = root + clean_path;
    
    if (script_path.find("..") != std::string::npos)
        return _sendError(res, 403, "Forbidden", &req);

    std::string script_dir;
    size_t last_slash = script_path.rfind('/');
    if (last_slash != std::string::npos)
        script_dir = script_path.substr(0, last_slash + 1);
    else
        script_dir = "./";

    std::string ext;
    size_t dot = script_path.rfind('.');
    if (dot != std::string::npos)
        ext = script_path.substr(dot);

    const std::map<std::string, std::string>& cgi_map = location.getCgiExtensions();
    std::map<std::string, std::string>::const_iterator it = cgi_map.find(ext);
    if (it == cgi_map.end())
        return _sendError(res, 500, "Internal Server Error", &req);
    std::string interpreter = it->second;

    struct stat st;
    if (stat(script_path.c_str(), &st) != 0)
        return _sendError(res, 404, "Not Found", &req);
    if (!S_ISREG(st.st_mode))
        return _sendError(res, 403, "Forbidden", &req);
    


    std::cout << "[cgi] script_path=" << script_path
          << " script_dir=" << script_dir
          << " interpreter=" << interpreter
          << " query_string=" << query_string << std::endl;
    
    std::vector<std::string> env = _buildCgiEnv(req, server, script_path, clean_path, query_string);
    for (size_t i = 0; i < env.size(); ++i) 
        std::cout << "[env] " << env[i] << std::endl;

    int fds_stdin[2];
    int fds_stdout[2];
    if (pipe(fds_stdin) == -1)
    {
        return _sendError(res, 500, "Internal Server Error", &req);
    }
    if (pipe(fds_stdout) == -1)
    {
        close(fds_stdin[0]);
        close(fds_stdin[1]);
        return _sendError(res, 500, "Internal Server Error", &req);
    }
    int pid_child = fork();
    if (pid_child == -1)
    {
        close(fds_stdin[0]);
        close(fds_stdin[1]);
        close(fds_stdout[0]);
        close(fds_stdout[1]);
        return _sendError(res, 500, "Internal Server Error", &req);
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
        if (chdir(script_dir.c_str()) == -1)
            _exit(1);
        std::string script_name = script_path.substr(last_slash + 1);
        std::vector<char *> argv;
        argv.push_back(const_cast<char *>(interpreter.c_str()));
        argv.push_back(const_cast<char *>(script_name.c_str()));
        argv.push_back(NULL);
        std::vector<char *> envp;
        for (size_t i = 0; i < env.size(); ++i)
            envp.push_back(const_cast<char *>(env[i].c_str()));
        envp.push_back(NULL);
        execve(interpreter.c_str(), &argv[0], &envp[0]);
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
            return _sendError(res, 500, "Internal Server Error", &req);
        }
        CgiProcess *cgi = new CgiProcess(res.getConn(), req, stdin_pipe, stdout_pipe, pid_child);
        LOG_TRACE("fd conn _ServerCgi " << res.getFd() << "\n");
        LOG_TRACE("fd conn cggi " << cgi->clientConn()->fd() << "\n");
        ConnectionPool::getInstance().addCgi(cgi);
    }
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
