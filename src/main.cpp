/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 15:23:36 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/07/07 17:52:47 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "MultiplexerEpoll.hpp"
# include "MultiplexerPoll.hpp"
# include "MultiplexerSelect.hpp"
# include "ConnectionPool.hpp"
# include "Server.hpp"
# include "SocketListenner.hpp"

ConfigServerListen	createTestListenner(int port = 8080)
{
	ConfigServerListen	listenner;

	listenner.address = "0.0.0.0";
	listenner.port = port;
	listenner.is_ipv4 = true;
	listenner.backlog = 10;
	return listenner;
}

SocketListenner	*makeListenner(int port = 8080)
{
	ConfigServerListen	listenner_config = createTestListenner(port);
	return new SocketListenner(listenner_config, 1024);
}

int main()
{
	// ConnectionPool	&pool = ConnectionPool::getInstance();
	// pool.setMultiplexer(new MultiplexerEpoll());
	// Server	*server = new Server();
	// pool.addListenner(makeListenner(), server);
	// pool.waitConnections();
}
