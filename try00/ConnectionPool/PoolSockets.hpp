/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PoolSockets.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 23:06:06 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/07 23:43:13 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_POLLS_HPP
# define SOCKET_POLLS_HPP

# include <iostream>
# include <vector>
# include <algorithm>

# include "Socket.hpp"

class PoolSockets
{
	protected:
		std::vector<Socket*>	_sockets;
		std::vector<pollfd>		_pollfds;

	public:
		typedef std::vector<Socket*>::iterator	iterator;

		PoolSockets() {};
		~PoolSockets() {};

		size_t	size() const { return _sockets.size(); }

		pollfd	*pollFds() { return &_pollfds[0]; }

		iterator	begin() { return _sockets.begin(); }
		iterator	end() { return _sockets.end(); }

		void	add(Socket *socket)
		{
			if (_sockets.empty())
			{
				_sockets.push_back(socket);
				_pollfds.push_back(*socket);
				return;
			}
			std::vector<Socket*>::iterator	it = std::lower_bound(_sockets.begin(), _sockets.end(), socket);
			if (*it == socket)
			{
				std::cerr << "Unable to add socket '" << socket->fd() << "'. Is already in the pool.\n";
				return;
			}
			size_t	idx = it - _sockets.begin();
			_sockets.insert(it, socket);
			_pollfds.insert(_pollfds.begin() + idx, *socket);
		}

		void	remove(Socket *socket)
		{
			std::vector<Socket*>::iterator	it = std::lower_bound(_sockets.begin(), _sockets.end(), socket);
			if (it == _sockets.end()) return ;
			size_t	idx = it - _sockets.begin();
			_sockets.erase(it);
			_pollfds.erase(_pollfds.begin() + idx);
		}
};

#endif
