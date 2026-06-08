/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 00:53:35 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/06/08 01:35:53 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "ConnecionPool.hpp"

ConfigServerListen	createTestListenner()
{
	ConfigServerListen	listenner;

	listenner.setAddress("0.0.0.0");
	listenner.setPort(8080);

	listenner.setIs_unix(false);

	listenner.setBacklog(10);

	listenner.setIs_ssl(false);
	listenner.setIs_http2(false);
	listenner.setIs_default_server(false);

	listenner.setIs_reuseport(false);
	listenner.setIs_ipv6only(false);

	listenner.setIs_proxy_protocol(false);
	listenner.setIs_deferred(false);
	listenner.setIs_bind(false);

	listenner.setRcvbuf(0);
	listenner.setSndbuf(0);

	listenner.setKeepalive(ConfigServerListenKeepAlive::OFF);
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

		ConnecionPool		&conn_pool = ConnecionPool::getInstance();
		conn_pool.addListenner(new ListenSocket(listenner_config));
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
			struct for_each_listenner: public ConnecionPool::functor_for_each_listenner
			{
				void	operator() (ListenSocket &socket)
				{

				}
			};
			for_each_listenner	callback;
			conn_pool.forEachListenner(callback);
		}
	}
};

#endif
