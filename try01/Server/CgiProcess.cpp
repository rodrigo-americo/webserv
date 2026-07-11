
#include <cstdlib>
#include <sstream>
#include <sys/wait.h>

#include "CgiProcess.hpp"
#include "SocketConnection.hpp"
#include "PipeChannel.hpp"
#include "PipeChannel.hpp"
#include "HttpResponse.hpp"
#include "Logger.hpp"
#include "CgiResponseParse.hpp"
#include "PipeChannel.hpp"
# include "ConnectionPool.hpp"

CgiProcess::CgiProcess(SocketConnection *client, const HttpRequest &req, PipeChannel *stdin_pipe, PipeChannel *stdout_pipe, pid_t pid)
    : _client_conn(client), _stdin_pipe(stdin_pipe), _stdout_pipe(stdout_pipe), _child_pid(pid), _body_to_write(req.body), _body_write_offset(0),_stdout_buffer(), _start_time(time(NULL)), _stdout_closed(false), _stdin_closed(false), _request(req)
{
	LOG_TRACE("CgiProcessConstructor(conn(" << client->fd() << "), " << *stdin_pipe << ", " << *stdout_pipe << ", PID " << pid << ")");
}

CgiProcess::~CgiProcess()
{
	::kill(_child_pid, SIGKILL);
	waitpid(_child_pid, NULL, 0);
	ConnectionPool::removeFileDescriptor(_stdin_pipe);
	ConnectionPool::removeFileDescriptor(_stdout_pipe);
	ConnectionPool::removeCgi(this);
}

void	CgiProcess::timeoutResponse()
{
	HttpResponse res(_client_conn);
    HttpResponseError err(res, 504, "Gateway Timeout", NULL);
    err.headers.content_type("text/html");
	err.send(ResponseHTTPVersion::HTTP_1_1);
	ConnectionPool::updateWriteInterest(_client_conn, _client_conn->hasPendingWrite());
	delete this;
}

void CgiProcess::onStdinWritable()
{
	LOG_TRACE("CGI begin write.");
    if (_body_write_offset >= _body_to_write.size())
	{
		LOG_TRACE("CGI cannot write: _body_write_offset(" << _body_write_offset << "), _body_to_write.size(" << _body_to_write.size() << ")" );
        return;
	}

    size_t remaining = _body_to_write.size() - _body_write_offset;
	LOG_TRACE("CGI writting on " << *_stdin_pipe);
    ssize_t n = ::write(_stdin_pipe->fd(), _body_to_write.data() + _body_write_offset, remaining);
    if (n < 0)
    {
		LOG_TRACE("CGI nothing to write on: " << *_stdin_pipe << "scheduling closing");
        _stdout_closed = true;
        return;
    }
    _body_write_offset += n;

}
void CgiProcess::onStdoutReadable()
{
    static const size_t buffer_size = 1024;
	LOG_TRACE("CGI reading from: " << *_stdout_pipe);
    ssize_t n = _stdout_pipe->read(buffer_size, _stdout_buffer);
    if (n <= 0)
	{
		LOG_TRACE("CGI nothing to read from : " << *_stdout_pipe << "scheduling closing");
        _stdout_closed = true;
	}
	LOG_TRACE("CGI data: " << _stdout_buffer);
}
bool CgiProcess::isDone() const { return _stdout_closed; }

bool CgiProcess::isExpired(time_t now, time_t timeout_secs) const
{
    return (now - _start_time) > timeout_secs;
}

static SetCookie parseSetCookieValue(const std::string& value)
{
    SetCookie cookie;
    size_t start = 0;
    while (start <= value.size())
    {
        size_t semi = value.find(';', start);
        utils::str segment = (semi == utils::str::npos) ? value.substr(start) : value.substr(start, semi - start);
        segment.trim();
        if (!segment.empty())
        {
            size_t eq = segment.find('=');
            if (eq == utils::str::npos)
                cookie.addKeyValue(segment.string(), "");
            else
                cookie.addKeyValue(segment.substr(0, eq).string(), segment.substr(eq + 1).string());
        }
        if (semi == utils::str::npos) break;
        start = semi + 1;
    }
    return cookie;
}

static void applyCgiHeaders(HttpResponse& res, const CgiResponseParse& parsed)
{
    for (size_t i = 0; i < parsed.getHeaders().size(); ++i)
    {
        const utils::str& key = (parsed.getHeaders())[i].first;
        const utils::str& value = (parsed.getHeaders())[i].second;
        if (key.tolower_const() == "set-cookie")
            res.headers.addSetCookie(parseSetCookieValue(value.string()));
        else
            (res.headers)[key.string()] = value.string();
    }
}

void CgiProcess::buildAndSendResponse()
{
    CgiResponseParse parsed(_stdout_buffer);
    HttpResponse res(_client_conn);

    if (parsed.isMalformed())
    {
        res.statusCode(502, "Bad Gateway");
        res.body("CGI output malformed\n");
        res.send(ResponseHTTPVersion::HTTP_1_1);
        ConnectionPool::updateWriteInterest(_client_conn, _client_conn->hasPendingWrite());
        return;
    }
    res.statusCode(parsed.getStatusCode(), parsed.getStatusMsg());
    applyCgiHeaders(res, parsed);
    res.body(parsed.getBody());
    res.send(ResponseHTTPVersion::HTTP_1_1);
	ConnectionPool::updateWriteInterest(_client_conn, _client_conn->hasPendingWrite());
	delete this;
}
