#include "WebServer.hpp"
#include "ConnectionPool.hpp"
#include "SocketListenner.hpp"
#include "MultiplexerEpoll.hpp"
#include "MultiplexerPoll.hpp"
#include "MultiplexerSelect.hpp"
#include "EventsConfig.hpp"
#include "GlobalConfig.hpp"
#include "ServerConfig.hpp"

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

	if (type == IO_POLL)  return new MultiplexerPoll();
	if (type == IO_EPOLL) return new MultiplexerEpoll();
	return new MultiplexerSelect();
}

void WebServer::start(WebServerConfig* config)
{
	size_t worker_connections = config->getWorkerConnections();
	_multiplexer = _createMultiplexer(config);

	ConnectionPool& pool = ConnectionPool::getInstance();
	pool.setMultiplexer(_multiplexer);

	const std::list<ServerConfig*>& servers = config->getServers();
	for (std::list<ServerConfig*>::const_iterator it = servers.begin(); it != servers.end(); it++)
	{
		Server* srv = new Server();
		_servers.push_back(srv);
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