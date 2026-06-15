/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionPool.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 01:54:10 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/11 17:04:26 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_POOL_HPP
# define CONNECTION_POOL_HPP

# include <map>
# include <vector>

# include "singleton.hpp"
# include "IMultiplexer.hpp"
# include "Server.hpp"
# include "HttpRequestBuilder.hpp"


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
	std::vector<RequestBuilder>	_pending_request;

	bool	_handleRequest(RequestBuilder &req_builder)
	{
		static const size_t buffer_size = 1024;
		std::string	buff;
		ssize_t bytes_read = req_builder.connection->read(buffer_size, buff);
		while (bytes_read == buffer_size)
			bytes_read = req_builder.connection->read(buffer_size, buff);

		req_builder.addToBuffer(buff);

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
						continue;
					}
					SocketConnection	*connection = new SocketConnection(event.socket);
					if (!connection->isValid()) { delete connection; continue; }
					_multiplexer->add(connection);
					continue;
				}
				// socket is SocketType::CONNETION

				SocketConnection	*conn = static_cast<SocketConnection*>(event.socket);
				RequestBuilder	req_builder(conn);
				if (_handleRequest(req_builder))
				{
					_multiplexer->remove(conn);
					continue;
				}
				// _pending_request.push_back(req_builder);
			}
		}
	}
};

#endif
