/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Try00PoolSockets.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 23:06:06 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/09 11:19:01 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_POLLS_HPP
# define SOCKET_POLLS_HPP

# include <iostream>
# include <vector>
# include <algorithm>

# include "Try00Socket.hpp"

class Try00PoolSockets
{
	protected:
		std::vector<Try00Socket*>	_sockets;
		std::vector<pollfd>		_pollfds;

	public:
		typedef std::vector<Try00Socket*>::iterator	iterator;

		Try00PoolSockets() {};
		~Try00PoolSockets() {};

		size_t	size() const { return _sockets.size(); }

		pollfd	*pollFds() { return &_pollfds[0]; }

		iterator	begin() { return _sockets.begin(); }
		iterator	end() { return _sockets.end(); }

		void	add(Try00Socket *socket)
		{
			if (_sockets.empty())
			{
				_sockets.push_back(socket);
				_pollfds.push_back(*socket);
				return;
			}
			std::vector<Try00Socket*>::iterator	it = std::lower_bound(_sockets.begin(), _sockets.end(), socket);
			if (*it == socket)
			{
				std::cerr << "Unable to add socket '" << socket->fd() << "'. Is already in the pool.\n";
				return;
			}
			size_t	idx = it - _sockets.begin();
			_sockets.insert(it, socket);
			_pollfds.insert(_pollfds.begin() + idx, *socket);
		}

		void	remove(Try00Socket *socket)
		{
			std::vector<Try00Socket*>::iterator	it = std::lower_bound(_sockets.begin(), _sockets.end(), socket);
			if (it == _sockets.end()) return ;
			size_t	idx = it - _sockets.begin();
			_sockets.erase(it);
			_pollfds.erase(_pollfds.begin() + idx);
		}
};

#endif
