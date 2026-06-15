/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Try00ConnecionPool.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 20:54:50 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/09 11:18:51 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_POOL_HPP
# define CONNECTION_POOL_HPP

# include <vector>
# include <algorithm>

# include "singleton.hpp"
# include "Try00Socket.hpp"
# include "Try00PoolSockets.hpp"
# include "Try00ListenSocket.hpp"
# include "Try00ConnectionSocket.hpp"

class Try00ConnecionPool: public patterns::singleton<Try00ConnecionPool>
{
	friend class patterns::singleton<Try00ConnecionPool>;
	private:
		Try00ConnecionPool() {};
		~Try00ConnecionPool() {};
		Try00ConnecionPool(const Try00ConnecionPool&);
		Try00ConnecionPool	&operator=(const Try00ConnecionPool&);

		Try00PoolSockets	_listenners;
		Try00PoolSockets	_connections;

	public:

	void	addListenner(Try00ListenSocket	*listenner)
	{
		_listenners.add(listenner);
	}

	void	connect(Try00ConnectionSocket	*connection)
	{
		_connections.add(connection);
	}

	void	disconnect(Try00ConnectionSocket	*connection)
	{
		_connections.remove(connection);
	}

	struct functor_for_each_listenner
	{
		virtual void	operator() (Try00ListenSocket &) = 0;
	};

	struct functor_for_each_connection
	{
		virtual void	operator() (Try00ConnectionSocket &) = 0;
	};


	pollfd	*listennersData() { return _listenners.pollFds(); }
	size_t	listennersSize() const { return _listenners.size(); }
	pollfd	*connectionsData() { return _connections.pollFds(); }
	size_t	connectionsSize() const { return _connections.size(); }

	void	forEachListenner(functor_for_each_listenner &callback)
	{
		for (Try00PoolSockets::iterator	it = _listenners.begin(); it != _listenners.end(); it++)
		{
			Try00ListenSocket	*socket = static_cast<Try00ListenSocket *>(*it);
			callback(*socket);
		}
	}

	void	forEachListenner(functor_for_each_connection &callback)
	{
		for (Try00PoolSockets::iterator	it = _connections.begin(); it != _connections.end(); it++)
		{
			Try00ConnectionSocket	*socket = static_cast<Try00ConnectionSocket *>(*it);
			callback(*socket);
		}
	}

};

#endif
