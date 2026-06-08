/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnecionPool.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 20:54:50 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/08 01:30:27 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_POOL_HPP
# define CONNECTION_POOL_HPP

# include <vector>
# include <algorithm>

# include "singleton.hpp"
# include "Socket.hpp"
# include "PoolSockets.hpp"
# include "ListenSocket.hpp"
# include "ConnectionSocket.hpp"

class ConnecionPool: public patterns::singleton<ConnecionPool>
{
	friend class patterns::singleton<ConnecionPool>;
	private:
		ConnecionPool() {};
		~ConnecionPool() {};
		ConnecionPool(const ConnecionPool&);
		ConnecionPool	&operator=(const ConnecionPool&);

		PoolSockets	_listenners;
		PoolSockets	_connections;

	public:

	void	addListenner(ListenSocket	*listenner)
	{
		_listenners.add(listenner);
	}

	void	connect(ConnectionSocket	*connection)
	{
		_connections.add(connection);
	}

	void	disconnect(ConnectionSocket	*connection)
	{
		_connections.remove(connection);
	}

	struct functor_for_each_listenner
	{
		virtual void	operator() (ListenSocket &) = 0;
	};

	struct functor_for_each_connection
	{
		virtual void	operator() (ConnectionSocket &) = 0;
	};


	pollfd	*listennersData() { return _listenners.pollFds(); }
	size_t	listennersSize() const { return _listenners.size(); }
	pollfd	*connectionsData() { return _connections.pollFds(); }
	size_t	connectionsSize() const { return _connections.size(); }

	void	forEachListenner(functor_for_each_listenner &callback)
	{
		for (PoolSockets::iterator	it = _listenners.begin(); it != _listenners.end(); it++)
		{
			ListenSocket	*socket = static_cast<ListenSocket *>(*it);
			callback(*socket);
		}
	}

	void	forEachListenner(functor_for_each_connection &callback)
	{
		for (PoolSockets::iterator	it = _connections.begin(); it != _connections.end(); it++)
		{
			ConnectionSocket	*socket = static_cast<ConnectionSocket *>(*it);
			callback(*socket);
		}
	}

};

#endif
