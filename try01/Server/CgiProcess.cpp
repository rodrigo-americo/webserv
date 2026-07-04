/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiProcess.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/28 23:18:30 by ighannam          #+#    #+#             */
/*   Updated: 2026/07/03 19:20:11 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdlib>
#include <sstream>

#include "CgiProcess.hpp"
#include "SocketConnection.hpp"
#include "SocketPipeRead.hpp"
#include "SocketPipeWrite.hpp"
#include "HttpResponse.hpp"
#include "Logger.hpp"

CgiProcess::CgiProcess(SocketConnection *client, const HttpRequest &req, SocketPipeWrite *stdin_pipe, SocketPipeRead *stdout_pipe, pid_t pid)
    : _client_conn(client), _stdin_pipe(stdin_pipe), _stdout_pipe(stdout_pipe), _child_pid(pid), _body_to_write(req.body), _body_write_offset(0),_stdout_buffer(), _start_time(time(NULL)), _stdout_closed(false), _stdin_closed(false), _request(req)
{
    
}

CgiProcess::~CgiProcess() {}

void CgiProcess::onStdinWritable()
{
    if (_body_write_offset >= _body_to_write.size())
        return;
    
    size_t remaining = _body_to_write.size() - _body_write_offset;
    ssize_t n = ::write(_stdin_pipe->fd(), _body_to_write.data() + _body_write_offset, remaining);
    if (n < 0)
    {
        _stdout_closed = true;
        return;
    }
    _body_write_offset += n;
     
}
void CgiProcess::onStdoutReadable()
{
    static const size_t buffer_size = 1024;
    ssize_t n = _stdout_pipe->read(buffer_size, _stdout_buffer);
    if (n <= 0)
        _stdout_closed = true;
}
bool CgiProcess::isDone() const { return _stdout_closed; }

bool CgiProcess::isExpired(time_t now, time_t timeout_secs) const
{
    return (now - _start_time) > timeout_secs;
}

void CgiProcess::buildAndSendResponse()
{
    size_t delim_pos = _stdout_buffer.find("\r\n\r\n");
    size_t delim_size = 4;
    if (delim_pos == std::string::npos)
    {
        delim_pos = _stdout_buffer.find("\n\n");
        delim_size = 2;
    }
    if (delim_pos == std::string::npos)
    {
        HttpResponse err(_client_conn);
        err.statusCode(502, "Bad Gateway");
        err.body("CGI output malformed\n");
        err.send(ResponseHTTPVersion::HTTP_1_1);
        return;
    }
    std::string headers = _stdout_buffer.substr(0, delim_pos);
    std::string body = _stdout_buffer.substr(delim_pos + delim_size);
    std::istringstream iss(headers);
    std::string line;
    int status_code = 200;
    std::string status_msg = "OK";
    HttpResponse res(_client_conn);
    res.statusCode(status_code, status_msg);
    while (std::getline(iss, line))
    {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        if (line.empty()) continue;
        
        size_t colon = line.find(':');
        if (colon == std::string::npos)
            continue;
        
        std::string key = line.substr(0, colon);
        std::string value = line.substr(colon + 1);
        
        while (!value.empty() && (value[0] == ' ' || value[0] == '\t'))
            value.erase(0, 1);
        
        if (key == "Status")
        {
            size_t space = value.find(' ');
            if (space != std::string::npos)
            {
                int code = std::atoi(value.substr(0, space).c_str());
                std::string msg = value.substr(space + 1);
                res.statusCode(code, msg);
            }
        }
        else
            res.headers[key] = value;
    }
    res.body(body);
    res.send(ResponseHTTPVersion::HTTP_1_1);

}