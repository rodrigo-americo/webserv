/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiProcess.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/28 23:18:30 by ighannam          #+#    #+#             */
/*   Updated: 2026/06/29 19:05:02 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiProcess.hpp"
#include "SocketConnection.hpp"
#include "SocketPipeRead.hpp"
#include "SocketPipeWrite.hpp"
#include "HttpResponse.hpp"
#include "Logger.hpp"

CgiProcess::CgiProcess(SocketConnection *client, const HttpRequest &req, SocketPipeWrite *stdin_pipe, SocketPipeRead *stdout_pipe, pid_t pid)
    : _client_conn(client), _stdin_pipe(stdin_pipe), _stdout_pipe(stdout_pipe), _child_pid(pid), _body_to_write(req.body), _body_write_offset(0),_stdout_buffer(), _start_time(time(NULL)), _stdout_closed(false), _request(req)
{
    (void)_body_write_offset;
    LOG_TRACE("fd conn CgiProcess contructor " << client->fd() << "\n");
}

CgiProcess::CgiProcess(const CgiProcess& other) : _client_conn(other._client_conn), _stdin_pipe(other._stdin_pipe), _stdout_pipe(other._stdout_pipe), _child_pid(other._child_pid), _body_to_write(other._body_to_write), _body_write_offset(other._body_write_offset),_stdout_buffer(other._stdout_buffer), _start_time(other._start_time), _stdout_closed(other._stdout_closed), _request(other._request)
{
    LOG_TRACE("copy contructor called " << other._client_conn->fd() << "\n");
}

CgiProcess::~CgiProcess() {}

void CgiProcess::onStdinWritable()
{
    // Implementar:
    // - Se `_body_write_offset >= _body_to_write.size()`, body já foi todo escrito.
    // Desregistra o pipe-write (precisa pedir ao pool — ver detalhes abaixo).
    // - Senão: chama `::write(_stdin_pipe->fd(), data + offset, remaining)`.
    // - Avança `_body_write_offset` pelo retorno.
    // - Se retorno == -1, problema (pode ser EAGAIN apesar do "ready" — defender
    // contra spurious wakeup) ou EPIPE (filho fechou stdin).
    
    if (_body_write_offset >= _body_to_write.size())
    {
        //retira o socket-pipe-write
    }
    else
    {
        _body_write_offset += ::write(_stdin_pipe->fd(), _body_to_write.c_str() + _body_write_offset, _body_to_write.size() - _body_write_offset);
    }    
}
void CgiProcess::onStdoutReadable()
{
    static const size_t buffer_size = 1024;
    while (_stdout_pipe->read(buffer_size, _stdout_buffer) == buffer_size)
    {
        /* code */
    }
    _stdout_closed = true;
    // std::cout << _stdout_buffer << "\n";
    
    if (_stdout_closed)
    {
        LOG_TRACE("fd conn onStdoutReadable " << _client_conn->fd() << "\n");
        buildAndSendResponse();
    }
    // - Chama `_stdout_pipe->read(buffer_size, _stdout_buffer)` num loop até
    // retornar 0 (EOF) ou < buffer_size (drenou).
    // - Se `read` retornar 0, marca `_stdout_closed = true`.
    // - (Alternativa: respeitar `event.eof` vindo do Poll/Epoll. Mas como o handler
    // não recebe o `SocketEvent` direto, mais simples confiar no `read` retornar 0.)

    // A signatura atual de `onStdoutReadable()` não recebe parâmetros. Se quiser
    // passar `event.eof`, precisa mudar a assinatura. Recomendado deixar como está
    // e detectar EOF via `read()==0`.
}
bool CgiProcess::isDone() const { return _stdout_closed; }

bool CgiProcess::isExpired(time_t now, time_t timeout_secs) const
{
    return (now - _start_time) > timeout_secs;
}

void CgiProcess::kill()
{
    
}
void CgiProcess::buildAndSendResponse()
{
    size_t body_start = _stdout_buffer.find("\r\n\r\n");
    if (body_start == std::string::npos)
    {
        body_start = _stdout_buffer.find("\n\n");
    }
    std::string header = _stdout_buffer.substr(0, body_start - 1);
    std::string body = _stdout_buffer.substr(body_start + 2, _stdout_buffer.size());
    LOG_TRACE("fd conn buildAndSendResponse " << _client_conn->fd() << "\n");
    HttpResponse res(_client_conn);
    res.statusCode(200, "ok!");
    res.body(body);
    res.headers.content_type("text/html");
    // res.headers.from(header);
    LOG_TRACE("fd REQ conn buildAndSendResponse " << _request.connection()->fd() << "\n");
    // res.getConn()->fd(3);
    res.send(ResponseHTTPVersion::HTTP_1_1);
    std::cout << res.getFd() << "\n";
    std::cout << "_client_conn: " << _client_conn->fd() << "\n";
    
    
    
    // Acionado pelo pool quando `cgi->isDone()` (atualmente: `_stdout_closed`).

    // Lógica:
    // 1. Parsear `_stdout_buffer`:
    // - Encontrar `\r\n\r\n` (ou `\n\n` como fallback).
    // - Tudo antes é bloco de headers CGI; tudo depois é body.
    // - Headers CGI seguem formato `Chave: Valor\r\n`.
    // 2. Para cada header parseado:
    // - Se chave for `Status`, o valor é o status code/message da resposta HTTP
    //     (ex: `Status: 302 Found`).
    // - Se chave for `Content-Type`, `Location`, etc, propagar para a response.
    // 3. Se não houve `Status:`, default 200 OK.
    // 4. Montar `HttpResponse` em cima do `_client_conn`, popular headers, body,
    // `res.send(HTTP_1_1)`.

    // Atenção: a `HttpResponse` é construída a partir do `SocketConnection*`. O
    // `CgiProcess` tem `_client_conn` (passado no construtor). Use ele, não
    // `_request._connection` (esse último é o ponteiro copiado — risco anotado
    // no item 14 do caderno).
}