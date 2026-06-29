/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionPool.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ighannam <ighannam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 01:54:10 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/29 18:22:27 by ighannam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_POOL_HPP
# define CONNECTION_POOL_HPP

# include <map>
# include <list>
# include <vector>

# include "singleton.hpp"
# include "IMultiplexer.hpp"
# include "Server.hpp"
# include "HttpRequestBuilder.hpp"
#include "CgiProcess.hpp"
#include "SocketPipeRead.hpp"
#include "SocketPipeWrite.hpp"
#include "Logger.hpp"


class HttpRequestObservers
{
	private:
		std::map<const Socket*, Server*>	_observers;

	public:
		HttpRequestObservers() {}
		~HttpRequestObservers() {}

		void	addSocketToObserver(const Socket *socket, Server *server)
		{
			_observers[socket] = server;
		}

		bool	notifyRequest(RequestBuilder &req_builder)
		{
			std::map<const Socket*, Server*>::iterator list_it = _observers.find(req_builder.connection->listenner());
			if (list_it == _observers.end())
			{
				std::cerr << "Error on handling request, listenner '" << req_builder.connection->listenner()->fd() << "' not found!" << std::endl;
				return false;
			}
			HttpRequest		req = req_builder.build();
			HttpResponse	res(req_builder.connection);
			list_it->second->handleRequest(req, res);
			return true;
		}
};

/**
 * ConnectionPool sera:
 *
 * - um `singleton`;
 * - um `OberverSubject` e os `Servers` serão os `Observers` dele;
 */
class ConnectionPool: public patterns::singleton<ConnectionPool>
{
	friend class patterns::singleton<ConnectionPool>;
private:
	IMultiplexer				*_multiplexer;
	HttpRequestObservers		_http_request_observers;
	std::list<RequestBuilder>	_pending_request;
	std::list<CgiProcess*>		_running_cgis;

	RequestBuilder *_findPending(SocketConnection *conn)
	{
		for (std::list<RequestBuilder>::iterator it = _pending_request.begin(); it != _pending_request.end(); ++it)
		{
			if (it->connection == conn)
				return &(*it);
		}
		return NULL;
	}

	CgiProcess *_findCgiByPipe(Socket *pipe_socket)
	{
		for (std::list<CgiProcess*>::iterator it = _running_cgis.begin();
			it != _running_cgis.end(); ++it)
		{
			if ((*it)->stdinPipe() == pipe_socket ||
				(*it)->stdoutPipe() == pipe_socket)
			{
				LOG_TRACE("fd conn _findCgiByPipe " << (*it)->clientConn()->fd() << "\n");
				return *it;
			}	
		}
		return NULL;
	}

	void	_removePending(SocketConnection *conn)
	{
		for (std::list<RequestBuilder>::iterator it = _pending_request.begin(); it != _pending_request.end(); ++it)
		{
			if (it->connection == conn)
			{
				_pending_request.erase(it);
				return;
			}
		}
	}

	bool	_handleRequest(RequestBuilder &req_builder)
	{
		static const size_t buffer_size = 1024;
		std::string	chunk;
		ssize_t bytes_read = req_builder.connection->read(buffer_size, chunk);
		while (bytes_read > 0)
		{
			req_builder.addToBuffer(chunk);
			if (req_builder.isComplete()) break;
			if (bytes_read < static_cast<ssize_t>(buffer_size)) break;
			chunk.clear();
			bytes_read = req_builder.connection->read(buffer_size, chunk);
		}

		bool	is_request_complete = req_builder.isComplete();
		if (is_request_complete)
			_http_request_observers.notifyRequest(req_builder);
		return is_request_complete;
	}

	// void	_processPendingRequests()
	// {
	// 	for (std::vector<RequestBuilder>::iterator it = _pending_request.begin(); it < _pending_request.end();)
	// 	{
	// 		if (_handleRequest(*it))
	// 		{
	// 			SocketConnection *conn = it->connection;
	// 			it = _pending_request.erase(it);
	// 			_multiplexer->remove(conn);
	// 			continue;
	// 		}
	// 		++it;
	// 	}
	// }

public:
	ConnectionPool(): _multiplexer(NULL) { }
	~ConnectionPool() {}

	void setMultiplexer(IMultiplexer *multiplexer)
	{
		_multiplexer = multiplexer;
	}

	bool	addListenner(Socket *socket, Server *server)
	{
		if (!socket || !socket->isValid()) return false;
		_multiplexer->add(socket);
		_http_request_observers.addSocketToObserver(socket, server);
		return true;
	}

	bool addPipe(Socket *pipe_socket)
	{
		if (!pipe_socket || !pipe_socket->isValid()) 
			return false;
		_multiplexer->add(pipe_socket);
		return true;
	}

	void	waitConnections()
	{
		std::cout << "Waitting for connections!\n";
		while (true)
		{
			// _processPendingRequests();
			SocketEventList	events;
			std::string error = _multiplexer->wait(events);
			if (!error.empty())
				std::cerr << error << std::endl;
			if (events.empty()) continue;
			for (size_t i = 0; i < events.size(); i++)
			{
				SocketEvent	event = events[i];

				if (event.socket->getType() == SocketType::LISTENNER)
				{
					if (!event.error.empty())
					{
						std::cerr << event.error << std::endl;
						_multiplexer->remove(event.socket);
						continue;
					}
					SocketConnection	*connection = new SocketConnection(event.socket);
					if (!connection->isValid()) { delete connection; continue; }
					_multiplexer->add(connection);
					continue;
				}
				else if (event.socket->getType() == SocketType::CONNECTION)
				{
					SocketConnection	*conn = static_cast<SocketConnection*>(event.socket);
					if (!event.error.empty() || event.eof)
					{
						_multiplexer->remove(conn);
						continue;
					}
					RequestBuilder *existing = _findPending(conn);
					if (existing)
					{
						if (_handleRequest(*existing))
						{
							_removePending(conn);
							_multiplexer->remove(conn);
						}
					}
					else
					{
						_pending_request.push_back(RequestBuilder(conn));
						if (_handleRequest(_pending_request.back()))
						{
							_pending_request.pop_back();
							_multiplexer->remove(conn);
						}
					}
				}
				else if (event.socket->getType() == SocketType::PIPE_READ)
				{
					CgiProcess *cgi = _findCgiByPipe(event.socket);
					LOG_TRACE("fd socket event: " << event.socket->fd());
					if (cgi)
					{
						LOG_TRACE("fd conn waitConnections " << cgi->clientConn()->fd() << "\n");
						cgi->onStdoutReadable();
						/* code */
					}
					
					
				}
				else if (event.socket->getType() == SocketType::PIPE_WRITE)
				{
					CgiProcess *cgi = _findCgiByPipe(event.socket);
					// LOG_TRACE("fd socket event: " << event.socket->fd());
					// LOG_TRACE("fd conn waitConnections " << cgi->clientConn()->fd() << "\n");
					cgi->onStdinWritable();
				}
				else
				{
					std::cerr << "unknown socket type " << event.socket->getType();
				}		
			}
		}
	}

	void addCgi(CgiProcess *cgi)
	{
		if (!cgi) return;
		_multiplexer->add(cgi->stdinPipe());
		_multiplexer->add(cgi->stdoutPipe());
		_running_cgis.push_back(cgi);
		LOG_TRACE("fd conn addCgi " << cgi->clientConn()->fd() << "\n");
	}
};

#endif
