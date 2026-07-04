
#include <cstdlib>
#include <sstream>

#include "CgiProcess.hpp"
#include "SocketConnection.hpp"
#include "SocketPipeRead.hpp"
#include "SocketPipeWrite.hpp"
#include "HttpResponse.hpp"
#include "Logger.hpp"
#include "CgiResponseParse.hpp"

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

// static SetCookie parseSetCookieValue(const std::string& value)
// {
//     SetCookie cookie;
//     size_t start = 0;
//     while (start <= value.size())
//     {
//         size_t semi = value.find(';', start);
//         utils::str segment = (semi == utils::str::npos) ? value.substr(start) : value.substr(start, semi - start);
//         segment.trim();
//         if (!segment.empty())
//         {
//             size_t eq = segment.find('=');
//             if (eq == utils::str::npos)
//                 cookie.addKeyValue(segment.string(), "");
//             else
//                 cookie.addKeyValue(segment.substr(0, eq).string(), segment.substr(eq + 1).string());
//         }
//         if (semi == utils::str::npos) break;
//         start = semi + 1;
//     }
//     return cookie;
// }

// static void applyCgiHeaders(HttpResponse& res, const CgiResponseParse& parsed)
// {
//     for (size_t i = 0; i < parsed.getHeaders().size(); ++i)
//     {
//         const utils::str& key = (parsed.getHeaders())[i].first;
//         const utils::str& value = (parsed.getHeaders())[i].second;
//         if (key.tolower_const() == "set-cookie")
//             res.headers.addSetCookie(parseSetCookieValue(value.string()));
//         else
//             (res.headers)[key.string()] = value.string();
//     }
// }

void CgiProcess::buildAndSendResponse()
{
    CgiResponseParse parsed(_stdout_buffer);
    HttpResponse res(_client_conn);

    if (parsed.isMalformed())
    {
        res.statusCode(502, "Bad Gateway");
        res.body("CGI output malformed\n");
        res.send(ResponseHTTPVersion::HTTP_1_1);
        return;
    }
    res.statusCode(parsed.getStatusCode(), parsed.getStatusMsg());
    //applyCgiHeaders(res, parsed);
    res.body(parsed.getBody());
    res.send(ResponseHTTPVersion::HTTP_1_1);

}