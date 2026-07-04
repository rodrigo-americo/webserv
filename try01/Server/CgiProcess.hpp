/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiProcess.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/28 22:19:23 by ighannam          #+#    #+#             */
/*   Updated: 2026/07/01 17:29:31 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIPROCESS_HPP
#define CGIPROCESS_HPP

#include <string>
#include <ctime>
#include <sys/types.h>

#include "HttpRequest.hpp"

class SocketConnection;
class SocketPipeRead;
class SocketPipeWrite;

class CgiProcess
{
private:
    SocketConnection *_client_conn;
    SocketPipeWrite  *_stdin_pipe;
    SocketPipeRead   *_stdout_pipe;
    pid_t _child_pid;
    std::string _body_to_write;
    size_t      _body_write_offset;
    std::string _stdout_buffer;
    time_t _start_time;
    bool   _stdout_closed;
    bool   _stdin_closed;
    HttpRequest _request;
    CgiProcess(const CgiProcess&);
    CgiProcess& operator=(const CgiProcess&);
public:
    CgiProcess(SocketConnection *client, const HttpRequest &req,
               SocketPipeWrite *stdin_pipe, SocketPipeRead *stdout_pipe,
               pid_t pid);
    ~CgiProcess();
    const std::string &stdoutBuffer() const { return _stdout_buffer; }
    size_t bodyWriteOffset()  const { return _body_write_offset; }
    size_t bodyToWriteSize()  const { return _body_to_write.size(); }
    void onStdinWritable();
    void onStdoutReadable();
    bool isDone() const;
    bool isExpired(time_t now, time_t timeout_secs) const;
    SocketPipeWrite *stdinPipe()  const { return _stdin_pipe; }
    bool stdinWriteFinished() const { return _body_write_offset >= _body_to_write.size(); }
    bool isStdinClosed()      const { return _stdin_closed; }
    void markStdinClosed()          { _stdin_closed = true; _stdin_pipe = NULL; }
    SocketPipeRead  *stdoutPipe() const { return _stdout_pipe; }
    pid_t            pid()        const { return _child_pid; }
    SocketConnection *clientConn() const { return _client_conn; }
    void buildAndSendResponse();
};




#endif
