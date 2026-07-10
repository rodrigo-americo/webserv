/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunofer <brunofer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/28 19:42:37 by ighannam          #+#    #+#             */
/*   Updated: 2026/07/10 19:23:02 by brunofer         ###   ########.fr       */
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
#include "utils.hpp"

#include <unistd.h>
#include <set>
#include "SocketPipeRead.hpp"

WebServer::WebServer() : _multiplexer(NULL) {}

WebServer::~WebServer()
{
	// for (size_t i = 0; i < _listeners.size(); i++)
	// 	delete _listeners[i];
	for (size_t i = 0; i < _servers.size(); i++)
		delete _servers[i];
	delete _multiplexer;
}

IMultiplexer* WebServer::_createMultiplexer()
{
	WebServerConfig	&config = WebServerConfig::getInstance();
	const GlobalConfig* global = config.getGlobal();
	const EventsConfig* events = global ? global->getEvents() : NULL;
	IOMultiplexer type = events ? events->getUse() : IO_EPOLL;

	IMultiplexer* mx;
	if (type == IO_POLL)  mx = new MultiplexerPoll();
	else if (type == IO_EPOLL) mx = new MultiplexerEpoll();
	else mx = new MultiplexerSelect();

	const HttpConfig* http = global ? global->getHttp() : NULL;
	size_t keepalive = http ? http->getKeepaliveTimeout() : 0;
	if (keepalive > 1000 || keepalive < 0)
		mx->setTimeout(static_cast<int>(1000));

	return mx;
}

void WebServer::start()
{
	WebServerConfig &config = WebServerConfig::getInstance();
	size_t worker_connections = config.getWorkerConnections();
	_multiplexer = _createMultiplexer();

	ConnectionPool& pool = ConnectionPool::getInstance();
	ConnectionPool::multiplexer(_multiplexer);

	Server* srv = new Server();
	_servers.push_back(srv);

	std::set<std::string> created_listens;

	const std::list<ServerConfig*>& servers = config.getServers();
	for (std::list<ServerConfig*>::const_iterator it = servers.begin(); it != servers.end(); it++)
	{
		const std::list<ConfigServerListen>& listens = (*it)->getListen();
		for (std::list<ConfigServerListen>::const_iterator lit = listens.begin(); lit != listens.end(); lit++)
		{
			std::string key = lit->is_unix
				? ("unix:" + lit->address)
				: (lit->address + ":" + utils::to_string(lit->port));
			if (!created_listens.insert(key).second)
			{
				std::cerr << "Duplicated listen ignored: " << key << std::endl;
				continue;
			}

			SocketListenner* sock = new SocketListenner(*lit, worker_connections);
			if (sock->hasErrors()) {
				created_listens.erase(key);
				delete sock;
				continue; }
			pool.addListenner(sock, srv);
		}
	if (created_listens.size() == 0)
	{
		std::cerr << "No active listenners. Shutting server down.\n";
		return;
	}
	}
	pool.waitConnections();
}
