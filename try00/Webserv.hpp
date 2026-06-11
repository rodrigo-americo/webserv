/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 00:53:35 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/10 21:45:45 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "Try00ConnecionPool.hpp"

ConfigServerListen	createTestListenner()
{
	ConfigServerListen	listenner;

	listenner.address = "0.0.0.0";
	listenner.port = 8080;
	listenner.is_ipv4 = true;
	listenner.backlog = 10;
}

class Webserv
{
	private:


	public:
		Webserv() {};
		~Webserv() {};

	void	loop() const
	{
		// isso vai vir pronto do parser
		ConfigServerListen	listenner_config = createTestListenner();

		Try00ConnecionPool		&conn_pool = Try00ConnecionPool::getInstance();
		conn_pool.addListenner(new Try00ListenSocket(listenner_config));
		while (true)
		{
			int ret = poll(conn_pool.listennersData(), conn_pool.listennersSize(), -1);
			if (ret < 0)
			{
				std::cerr << "listenners_poll() falhou!" << std::endl;
				break;
			}
			ret = poll(conn_pool.connectionsData(), conn_pool.connectionsSize(), -1);
			if (ret < 0)
			{
				std::cerr << "connections_poll() falhou!" << std::endl;
				break;
			}
			// talvez isso nao e uma boa ideia, da pra criar um metoo que ja cria as conexoes em vez de usar o forEach
			struct for_each_listenner: public Try00ConnecionPool::functor_for_each_listenner
			{
				void	operator() (Try00ListenSocket &socket)
				{

				}
			};
			for_each_listenner	callback;
			conn_pool.forEachListenner(callback);
		}
	}
};

#endif
