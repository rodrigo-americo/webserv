/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionPool.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 01:54:10 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/10 21:55:32 by bruno-valer      ###   ########.fr       */
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
	std::map<const Socket*, Server*>	_listenners;
	std::vector<RequestBuilder>	_pending_request;



	bool	_handleRequest(RequestBuilder &req_builder)
	{
		std::map<const Socket*, Server*>::iterator list_it = _listenners.find(req_builder.connection->listenner());
		if (list_it != _listenners.end())
		{
			HttpRequest		req = req_builder.build();
			HttpResponse	res(req_builder.connection);
			list_it->second->handleRequest(req, res);
			return true;
		}
		return false;
	}

	bool	_completeRequest(RequestBuilder &req_builder)
	{
		static const size_t buffer_size = 1024;
		std::string	buff;
		req_builder.connection->read(buffer_size, buff);
		req_builder.addToBuffer(buff, buff.size() < buffer_size);
		if (req_builder.isComplete())
		{
			std::map<const Socket*, Server*>::iterator list_it = _listenners.find(req_builder.connection->listenner());
			if (list_it == _listenners.end())
			{
				std::cerr << "Error on handling request, listenner '" << req_builder.connection->listenner()->fd() << "' not found!" << std::endl;
				return true;
			}
			HttpRequest		req = req_builder.build();
			HttpResponse	res(req_builder.connection);
			list_it->second->handleRequest(req, res);
			return true;
		}
		return false;
	}

	void	_processPendingRequests()
	{
		for (std::vector<RequestBuilder>::iterator it = _pending_request.begin(); it < _pending_request.end();)
		{
			if (_completeRequest(*it))
			{
				it = _pending_request.erase(it);
				_multiplexer->remove(it->connection);
				continue;
			}
			++it;
		}
	}

public:
	ConnectionPool(): _multiplexer(NULL) { }
	~ConnectionPool() {}

	void setMultiplexer(IMultiplexer *multiplexer)
	{
		_multiplexer = multiplexer;
	}

	void	addListenner(Socket *socket, Server *server)
	{
		_multiplexer->add(socket);
		_listenners[socket] = server;
	}

	void	waitConnections()
	{
		while (true)
		{
			SocketEventList	events;
			std::string error = _multiplexer->wait(events);
			if (!error.empty())
				std::cerr << error << std::endl;
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
					_multiplexer->add(new SocketConnection(event.socket));
					continue;
				}
				// socket is SocketType::CONNETION

				_processPendingRequests();
				SocketConnection	*conn = static_cast<SocketConnection*>(event.socket);
				RequestBuilder	req_builder(conn);
				if (_completeRequest(req_builder))
				{
					_multiplexer->remove(conn);
					continue;
				}
				_pending_request.push_back(req_builder);
			}
		}
	}
};

#endif
