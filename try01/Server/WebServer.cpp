/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/28 19:42:37 by ighannam          #+#    #+#             */
/*   Updated: 2026/06/28 20:59:50 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include "ConnectionPool.hpp"
#include "SocketListenner.hpp"
#include "MultiplexerEpoll.hpp"
#include "MultiplexerPoll.hpp"
#include "MultiplexerSelect.hpp"
#include "EventsConfig.hpp"
#include "GlobalConfig.hpp"
#include "HttpConfig.hpp"
#include "ServerConfig.hpp"

#include <unistd.h>
#include "SocketPipeRead.hpp"

WebServer::WebServer() : _multiplexer(NULL) {}

WebServer::~WebServer()
{
	for (size_t i = 0; i < _listeners.size(); i++)
		delete _listeners[i];
	for (size_t i = 0; i < _servers.size(); i++)
		delete _servers[i];
	delete _multiplexer;
}

IMultiplexer* WebServer::_createMultiplexer(const WebServerConfig* config)
{
	const GlobalConfig* global = config->getGlobal();
	const EventsConfig* events = global ? global->getEvents() : NULL;
	IOMultiplexer type = events ? events->getUse() : IO_EPOLL;

	IMultiplexer* mx;
	if (type == IO_POLL)  mx = new MultiplexerPoll();
	else if (type == IO_EPOLL) mx = new MultiplexerEpoll();
	else mx = new MultiplexerSelect();

	const HttpConfig* http = global ? global->getHttp() : NULL;
	size_t keepalive = http ? http->getKeepaliveTimeout() : 0;
	if (keepalive > 0)
		mx->setTimeout(static_cast<int>(keepalive * 1000));

	return mx;
}

void WebServer::start(WebServerConfig* config)
{
	size_t worker_connections = config->getWorkerConnections();
	_multiplexer = _createMultiplexer(config);

	ConnectionPool& pool = ConnectionPool::getInstance();
	pool.setMultiplexer(_multiplexer);

	Server* srv = new Server(config);
	_servers.push_back(srv);

	const std::list<ServerConfig*>& servers = config->getServers();
	for (std::list<ServerConfig*>::const_iterator it = servers.begin(); it != servers.end(); it++)
	{	
		const std::list<ConfigServerListen>& listens = (*it)->getListen();
		for (std::list<ConfigServerListen>::const_iterator lit = listens.begin(); lit != listens.end(); lit++)
		{
			SocketListenner* sock = new SocketListenner(*lit, worker_connections);
			if (sock->hasErrors()) { delete sock; continue; }
			_listeners.push_back(sock);
			pool.addListenner(sock, srv);
		}
	}
	pool.waitConnections();
}
