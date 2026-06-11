/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/10 15:23:36 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/10 21:45:58 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "MultiplexerEpoll.hpp"
# include "MultiplexerPoll.hpp"
# include "MultiplexerSelect.hpp"
# include "ConnectionPool.hpp"

ConfigServerListen	createTestListenner()
{
	ConfigServerListen	listenner;

	listenner.address = "0.0.0.0";
	listenner.port = 8080;
	listenner.is_ipv4 = true;
	listenner.backlog = 10;
}

int main()
{
	ConnectionPool	&pool = ConnectionPool::getInstance();
	pool.setMultiplexer(new MultiplexerEpoll());
	// pool.addListenner()
}
